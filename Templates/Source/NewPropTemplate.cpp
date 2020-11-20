static void RegisterObject(Context* context) {
	context->RegisterFactory<%s::%s>();
	URHO3D_ACCESSOR_ATTRIBUTE("Is Enabled", IsEnabled, SetEnabled, bool, true, AM_DEFAULT);
}