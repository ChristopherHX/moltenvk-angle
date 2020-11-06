#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Node.h>
#include <cling/Interpreter/Interpreter.h>
#include <cling/Interpreter/Value.h>
#include <cling/Utils/Casting.h>

namespace Urho3D
{

    class ClingComponentEntry : public Object {
        URHO3D_OBJECT(ClingComponentEntry, Object);

    public:
        /// Construct.
        explicit ClingComponentEntry(Context* context);
        /// Destruct.
        ~ClingComponentEntry() override;


        cling::Interpreter* _interpreter;
        String _fileName;
    };

    class URHO3D_API ClingComponentManager : public Object
    {
        URHO3D_OBJECT(ClingComponentManager, Object);

    public:
        /// Construct.
        explicit ClingComponentManager(Context* context);
        /// Destruct.
        ~ClingComponentManager() override;

        void Reset();

 

        void HandleFileChanged(StringHash eventType, VariantMap& eventData);


        bool  processComponent(String fileName);
        void setScene(Scene* scene) { _currentScene = scene; }

    private :
        ClingComponentEntry* createClingComponentEntry(String fileName);
        HashMap<String, SharedPtr<ClingComponentEntry> > _entries;
        Scene* _currentScene;

    };

}