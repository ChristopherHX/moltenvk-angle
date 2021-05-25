#include <fstream>

#include "barrier.h"
#include "crown.h"
#include "environment.h"
#include "fish.h"
#include "flappycam.h"
#include "global.h"
#include "score3d.h"
#include "weed.h"

#include "mastercontrol.h"

URHO3D_DEFINE_APPLICATION_MAIN(MasterControl);

MasterControl* MasterControl::instance_ = NULL;
MasterControl* MasterControl::GetInstance() { return instance_; }

void RegisterAdmobPlugin(Context* context);

MasterControl::MasterControl(Context* context)
    : Application(context)
    , scene_{nullptr}
    , drawDebug_{false}
{
    instance_ = this;

    SubscribeToEvent(E_BEGINFRAME, URHO3D_HANDLER(MasterControl, HandleBeginFrame));

    Barrier::RegisterObject(context);
    Crown::RegisterObject(context);
    Weed::RegisterObject(context);
    Fish::RegisterObject(context);
    Environment::RegisterObject(context);
    FlappyCam::RegisterObject(context);
    Score3D::RegisterObject(context);
}

void MasterControl::Setup()
{

    engineParameters_[EP_RESOURCE_PATHS] = "Data/FlappyUrho;Data;CoreData;";
    engineParameters_[EP_LOG_NAME] = GetSubsystem<FileSystem>()->GetProgramDir() + "FlappyUrho.log";
    engineParameters_[EP_FULL_SCREEN] = false;
    engineParameters_[EP_WINDOW_WIDTH] = 1280;
    engineParameters_[EP_WINDOW_HEIGHT] = 720;
    engineParameters_[EP_WINDOW_TITLE] = "Flappy Urho";
    engineParameters_[EP_WINDOW_ICON] = "icon.png";

    context_->RegisterSubsystem(new Global(context_));
}

void MasterControl::Start()
{

    RegisterAdmobPlugin(context_);
    isAdmobPluginStarted = false;
    isVideoAdLoaded = false;
    LoadRewardedVideo();

    SetRandomSeed(Time::GetSystemTime());

    CreateScene();
    CreateUI();

    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MasterControl, HandleUpdate));
    SubscribeToEvent(E_PLUGIN_NOTIFY, URHO3D_HANDLER(MasterControl, HandlePluginMessage));

    SoundSource* musicSource{scene_->GetOrCreateComponent<SoundSource>()};
    musicSource->SetSoundType(SOUND_MUSIC);
    Sound* music{CACHE->GetResource<Sound>("Music/Urho - Disciples of Urho_LOOP.ogg")};
    music->SetLooped(true);
    musicSource->Play(music);

    GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, 0.33f);
}

void MasterControl::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>();
    scene_->CreateComponent<DebugRenderer>();

    scene_->CreateComponent<PhysicsWorld>();

    Node* cameraNode{scene_->CreateChild("Camera")};
    cameraNode->CreateComponent<FlappyCam>();

    Zone* zone{cameraNode->CreateComponent<Zone>()};
    zone->SetBoundingBox(BoundingBox(-100.0f * Vector3::ONE, 100.0f * Vector3::ONE));
    zone->SetFogStart(34.0f);
    zone->SetFogEnd(62.0f);
    zone->SetFogHeight(-19.0f);
    zone->SetHeightFog(true);
    zone->SetFogHeightScale(0.1f);
    zone->SetFogColor(Color(0.05f, 0.23f, 0.23f));
    zone->SetAmbientColor(Color(0.05f, 0.13f, 0.13f));

    Node* lightNode{scene_->CreateChild()};
    Light* light{lightNode->CreateComponent<Light>()};
    light->SetLightType(LIGHT_DIRECTIONAL);
    light->SetCastShadows(true);
    light->SetShadowIntensity(0.23f);
    light->SetBrightness(1.23f);
    light->SetColor(Color(0.8f, 1.0f, 1.0f));
    lightNode->SetDirection(Vector3(-0.5f, -1.0f, 1.0f));

    Node* envNode{scene_->CreateChild()};
    Skybox* skybox{envNode->CreateComponent<Skybox>()};
    skybox->SetModel(CACHE->GetResource<Model>("Models/Box.mdl"));
    skybox->SetMaterial(CACHE->GetResource<Material>("Materials/Env.xml"));
    skybox->SetZone(zone);
    envNode->CreateComponent<Environment>();

    CreateUrho();
    CreateNets();
    CreateWeeds();
    CreateCrown();
}

void MasterControl::CreateUrho()
{
    Node* urhoNode{scene_->CreateChild("Urho")};
    urhoNode->CreateComponent<Fish>();
}

void MasterControl::CreateNets()
{
    for (int i{0}; i < NUM_BARRIERS; ++i)
    {
        Node* barrierNode{scene_->CreateChild("Barrier")};
        barrierNode->CreateComponent<Barrier>();
        barrierNode->SetPosition(Vector3(BAR_OUTSIDE_X * 1.23f + i * BAR_INTERVAL, BAR_RANDOM_Y, 0.0f));
    }
}
void MasterControl::CreateWeeds()
{
    for (int r{0}; r < 3; ++r)
    {
        for (int i{0}; i < NUM_WEEDS; ++i)
        {
            Node* weedNode{scene_->CreateChild("Weed")};
            weedNode->CreateComponent<Weed>();
            weedNode->SetPosition(Vector3(i * BAR_INTERVAL * Random(0.1f, 0.23f) - 23.0f, WEED_RANDOM_Y,
                                          Random(-27.0f + r * 34.0f, -13.0f + r * 42.0f)));

            weedNode->SetRotation(Quaternion(0.0f, Random(360.0f), 0.0f));
            weedNode->SetScale(Vector3(Random(0.5f, 1.23f), Random(0.8f, 2.3f), Random(0.5f, 1.23f)));
        }
    }
}

void MasterControl::CreateCrown()
{
    Node* crownNode{scene_->CreateChild("Crown")};
    crownNode->CreateComponent<Crown>();
}

void MasterControl::CreateUI()
{
    Font* font{CACHE->GetResource<Font>("Fonts/Ubuntu-BI.ttf")};

    Text* helpText{UI_ROOT->CreateChild<Text>()};
    helpText->SetFont(font, 20);
    helpText->SetTextEffect(TE_SHADOW);
    helpText->SetAlignment(HA_CENTER, VA_CENTER);
    helpText->SetPosition(0, UI_ROOT->GetHeight() / 4);
    helpText->AddTags("Intro;Dead");

    helpText->SetText("Left click to swim");

    Node* scoreNode{scene_->CreateChild("Score")};
    Node* highscoreNode{scene_->CreateChild("Highscore")};
    GLOBAL->SetScores3D(scoreNode->CreateComponent<Score3D>(), highscoreNode->CreateComponent<Score3D>());
}

void MasterControl::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    (void)eventType;
    (void)eventData;

    if (GLOBAL->gameState_ == GLOBAL->neededGameState_)
        return;

    if (GLOBAL->neededGameState_ == GS_DEAD)
    {

        Node* urhoNode{scene_->GetChild("Urho")};
        SoundSource* soundSource{urhoNode->GetOrCreateComponent<SoundSource>()};
        soundSource->Play(CACHE->GetResource<Sound>("Samples/Hit.ogg"));

        if (isVideoAdLoaded == false)
        {
            LoadRewardedVideo();
        }
    }
    else if (GLOBAL->neededGameState_ == GS_VIDEO_AD)
    {
        if (isVideoAdLoaded == true)
        {
            ShowRewardedVideo();
        }
        else
        {
            GLOBAL->neededGameState_ = GS_INTRO;
            return;
        }
    }
    else if (GLOBAL->neededGameState_ == GS_INTRO)
    {

        Node* urhoNode{scene_->GetChild("Urho")};
        Fish* fish{urhoNode->GetComponent<Fish>()};
        fish->Reset();

        Node* crownNode{scene_->GetChild("Crown")};
        Crown* crown{crownNode->GetComponent<Crown>()};
        crown->Reset();

        if (GLOBAL->GetScore() > GLOBAL->GetHighscore())
            GLOBAL->SetHighscore(GLOBAL->GetScore());
        GLOBAL->SetScore(0);
        GLOBAL->sinceLastReset_ = 0.0f;

        PODVector<Node*> barriers{};
        scene_->GetChildrenWithComponent<Barrier>(barriers);
        for (Node* b : barriers)
        {

            Vector3 pos{b->GetPosition()};
            pos.y_ = BAR_RANDOM_Y;

            if (pos.x_ < BAR_OUTSIDE_X)
                pos.x_ += NUM_BARRIERS * BAR_INTERVAL;

            b->SetPosition(pos);
        }

        PODVector<Node*> weeds{};
        scene_->GetChildrenWithComponent<Weed>(weeds);
        for (Node* w : weeds)
        {

            w->Remove();
        }
        CreateWeeds();
    }

    GLOBAL->gameState_ = GLOBAL->neededGameState_;

    UpdateUIVisibility();
}

void MasterControl::UpdateUIVisibility()
{
    String tag{};
    if (GLOBAL->gameState_ == GS_PLAY)
        tag = "Gameplay";
    else if (GLOBAL->gameState_ == GS_DEAD)
        tag = "Dead";
    else
        tag = "Intro";

    Vector<SharedPtr<UIElement>> uiElements{UI_ROOT->GetChildren()};
    for (UIElement* e : uiElements)
    {

        e->SetVisible(e->HasTag(tag));
    }
}

void MasterControl::HandlePluginMessage(StringHash eventType, VariantMap& eventData)
{
    String data = eventData[PluginNotify::P_DATA].GetString();

    auto jasonFile = MakeShared<JSONFile>(context_);
    jasonFile->FromString(data);
    auto root = jasonFile->GetRoot();
    String source = root["source"].GetCString();
    String event = root["event"].GetCString();

    URHO3D_LOGINFO("HandlePluginMessage source:" + source + " event:" + event);

    if(source == "AdmobPlugin" && event == "OnStarted")
    {
        // plugin started successfully , now we can load ads
        isAdmobPluginStarted = true;
        LoadRewardedVideo();
    }
    else if (event == "onAdLoaded")
    {
        // Video Ad loaded successfully
        isVideoAdLoaded = true;
    }
    else if (event == "onAdFailedToLoad")
    {
        // Video Ad failed to load
        isVideoAdLoaded = false;
    }
    else if (event == "onAdShowedFullScreenContent")
    {
        GLOBAL->neededGameState_ = GS_VIDEO_AD;
        // Mute the sound while the video ad is playing
        GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, 0.0f);
    }
    else if (event == "onAdDismissedFullScreenContent")
    {
        GLOBAL->neededGameState_ = GS_INTRO;
        // Unmute the sound once the video ad stopped playing
        GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, 0.33f);
        // Load new Video ad
        LoadRewardedVideo();
    }
    else if (event == "onAdFailedToShowFullScreenContent")
    {
        // Video Ad failed to show

        GLOBAL->neededGameState_ = GS_INTRO;
        GetSubsystem<Audio>()->SetMasterGain(SOUND_MUSIC, 0.33f);
        LoadRewardedVideo();
    }
    else if (event == "onUserEarnedReward")
    {
        //  video ad completed , user is rewarded

        String rewardType = root["rewardType"].GetCString();
        int rewardAmount = root["rewardAmount"].GetInt();

        URHO3D_LOGINFO("onUserEarnedReward rewardType:" + rewardType + " rewardAmount:" + String(rewardAmount));

    }
}


void MasterControl::LoadRewardedVideo()
{
    if(isAdmobPluginStarted == false)
    {
        // before loading ads , one must start the plugin
         PostCommandToPlugin("AdmobPlugin", "Start");
    }
    else if (isVideoAdLoaded == false)
    {
        // using test adUnitId
        auto jsonBuilder = (MakeShared<JsonBuilder>(context_));
        (*jsonBuilder)("adUnitId", "ca-app-pub-3940256099942544/1712485313");
        
        /*
                test device can be also configured
                (*jsonBuilder)("adUnitId", "ca-app-pub-3940256099942544/9793981313")("testDeviceId","2077ef9a63d2b398840261c8221a0c9b");
                More information on TestDeviceId
                https://developers.google.com/admob/android/test-ads
                https://developers.google.com/admob/ios/test-ads
        */
        PostCommandToPlugin("AdmobPlugin", "loadRewardedAd", jsonBuilder);
    }
}

void MasterControl::ShowRewardedVideo()
{

    if (isVideoAdLoaded)
    {
        isVideoAdLoaded = false;
        if (PostCommandToPlugin("AdmobPlugin", "showRewardedVideo") == false)
        {
            GLOBAL->neededGameState_ = GS_INTRO;
        }
    }
    else
    {
        GLOBAL->neededGameState_ = GS_INTRO;
    }
}

void MasterControl::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    (void)eventType;

    if (GLOBAL->gameState_ == GS_PLAY)
    {

        GLOBAL->sinceLastReset_ += eventData[Update::P_TIMESTEP].GetFloat();
    }
    scene_->SetTimeScale(Pow(1.0f + Clamp(GLOBAL->sinceLastReset_ * 0.0023f, 0.0f, 1.0f), 2.3f));
    SoundSource* musicSource{scene_->GetComponent<SoundSource>()};
    musicSource->SetFrequency(0.5f * (musicSource->GetFrequency() + 44000.0f * Sqrt(scene_->GetTimeScale())));

    if (INPUT->GetMouseButtonPress(MOUSEB_LEFT) || INPUT->GetNumTouches() > 0)
    {

        if (GLOBAL->gameState_ == GS_INTRO)
            GLOBAL->neededGameState_ = GS_PLAY;
        else if (GLOBAL->gameState_ == GS_DEAD)
            GLOBAL->neededGameState_ = GS_VIDEO_AD;
    }
    if (INPUT->GetKeyPress(KEY_9))
    {

        Image screenshot{GetContext()};
        GRAPHICS->TakeScreenShot(screenshot);
        // Here we save in the Data folder with date and time appended
        String fileName{GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                        Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_') + ".png"};
        Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    }
    if (INPUT->GetKeyPress(KEY_M))
    {

        SoundSource* musicSource{scene_->GetComponent<SoundSource>()};
        musicSource->SetEnabled(!musicSource->IsEnabled());
    }

    if (INPUT->GetKeyPress(KEY_ESCAPE))
    {

        engine_->Exit();
    }

    if (INPUT->GetKeyPress(KEY_SPACE))
    {

        drawDebug_ = !drawDebug_;
    }
}

void MasterControl::HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    (void)eventType;
    (void)eventData;

    if (drawDebug_)
    {

        scene_->GetComponent<PhysicsWorld>()->DrawDebugGeometry(true);
    }
}
