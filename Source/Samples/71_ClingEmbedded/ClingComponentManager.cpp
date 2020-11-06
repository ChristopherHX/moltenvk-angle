#include "ClingComponentManager.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "clang/Basic/FileSystemStatCache.h"
#include "clang/Basic/VirtualFileSystem.h"
#include "llvm/Support/Path.h"
#include "clang/Basic/VirtualFileSystem.h"
#include "clang/Basic/FileManager.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Twine.h"


#include <Urho3D/Core/Object.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Resource/ResourceEvents.h>
#include <Urho3D/Scene/Scene.h>

namespace Urho3D
{


	static Context* gContext = NULL;

	Urho3D::Context* getContext() { return gContext; }


	/// Construct.
	ClingComponentEntry::ClingComponentEntry(Context* context) :
		Object(context)
	{

	}
	/// Destruct.
	ClingComponentEntry::~ClingComponentEntry()
	{

	}


	ClingComponentManager::ClingComponentManager(Context* context) :
		Object(context)
	{
		gContext = context;
		Urho3D::ResourceCache* cache = GetSubsystem<Urho3D::ResourceCache>();
		cache->SetAutoReloadResources(true);

		SubscribeToEvent(cache, E_FILECHANGED, URHO3D_HANDLER(ClingComponentManager, HandleFileChanged));
	}

	ClingComponentManager::~ClingComponentManager()
	{






	}

	void ClingComponentManager::Reset()
	{
		for (HashMap<String, SharedPtr<ClingComponentEntry> >::ConstIterator j = _entries.Begin(); j != _entries.End(); ++j)
		{
			String fullName = j->first_;
			String path, name, ext;

			SplitPath(fullName, path, name, ext);

			PODVector<Node*> childeren = _currentScene->GetChildrenWithComponent(name, true);
			for (Node* child : childeren)
			{
 				child->RemoveComponent(name);
			}

			context_->RemoveFactory(name);

			delete j->second_->_interpreter;
			j->second_->_interpreter = NULL;
		}

		_entries.Clear();
	}

	ClingComponentEntry* ClingComponentManager::createClingComponentEntry(String fileName)
	{
		String path, name, ext;

		SplitPath(fileName, path, name, ext);
		String cmd = "";

		cling::Value res;
		const char* tt = "dummy";
		int argc = 1;
		const char* const* argv = &(tt);

		ClingComponentEntry* clingComponentEntry = new ClingComponentEntry(context_);

		clingComponentEntry->_interpreter = new cling::Interpreter(argc, argv, "D:/temp/cling/tools/packaging/cling-build/builddir/Release");
		clingComponentEntry->_interpreter->enableDynamicLookup(true);
		clingComponentEntry->_interpreter->allowRedefinition(true);


		clingComponentEntry->_interpreter->AddIncludePath("D:/Urho3D-Dev/master/Urho3D/build-vs2019/include");
		clingComponentEntry->_interpreter->AddIncludePath("D:/Urho3D-Dev/master/Urho3D/build-vs2019/include/Urho3D");
		clingComponentEntry->_interpreter->AddIncludePath("D:/Urho3D-Dev/master/Urho3D/build-vs2019/include/Urho3D/ThirdParty");
		clingComponentEntry->_interpreter->AddIncludePath("D:/Urho3D-Dev/master/Urho3D/build-vs2019/include/Urho3D/ThirdParty/Bullet");

		clingComponentEntry->_interpreter->AddIncludePath(path.CString());

		cling::Interpreter::CompilationResult compilationResult = clingComponentEntry->_interpreter->loadFile("D:/Urho3D-Dev/master/Urho3D/build_cling_lib/bin/Urho3D.dll");

		if (compilationResult != cling::Interpreter::CompilationResult::kSuccess)goto error;

		compilationResult = clingComponentEntry->_interpreter->loadHeader("Urho3D/Core/Context.h");
		if (compilationResult != cling::Interpreter::CompilationResult::kSuccess)goto error;

		compilationResult = clingComponentEntry->_interpreter->declare("Urho3D::Context* getContext();");
		if (compilationResult != cling::Interpreter::CompilationResult::kSuccess)goto error;

		compilationResult = clingComponentEntry->_interpreter->loadFile(fileName.CString());
		if (compilationResult != cling::Interpreter::CompilationResult::kSuccess)goto error;


		cmd = "getContext()->RegisterFactory<" + name + ">();";
		compilationResult = clingComponentEntry->_interpreter->process(cmd.CString());
		if (compilationResult != cling::Interpreter::CompilationResult::kSuccess)goto error;


		return clingComponentEntry;

	error:
		delete clingComponentEntry;
		return NULL;

	}

	bool  ClingComponentManager::processComponent(String fileName)
	{

		bool result = false;

		String path, name, ext;

		SplitPath(fileName, path, name, ext);

		ClingComponentEntry* clingComponentEntry = _entries[fileName];
		if (clingComponentEntry == NULL)
		{
			clingComponentEntry = createClingComponentEntry(fileName);
			if (clingComponentEntry)
			{

				_entries[fileName] = clingComponentEntry;
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else
		{

			clingComponentEntry = createClingComponentEntry(fileName);
			if (clingComponentEntry)
			{
				PODVector<Node*> childeren = _currentScene->GetChildrenWithComponent(name, true);

				for (Node* child : childeren)
				{
					child->RemoveComponent(name);
					child->CreateComponent(name);

				}
				_entries[fileName] = clingComponentEntry;

				result = true;
			}
			else
			{
				result = false;
			}


		}

		return result;
	}


	void ClingComponentManager::HandleFileChanged(StringHash eventType, VariantMap& eventData)
	{
		String filename = eventData["FileName"].GetString();
		if (_entries[filename] != NULL) processComponent(filename);

	}


}
