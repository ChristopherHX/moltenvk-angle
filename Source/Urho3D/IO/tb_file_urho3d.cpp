
#include "Urho3D.h"

#include "../Engine/Application.h"

#include "../Urho3D/Core/Context.h"

#include "../Urho3D/Core/Object.h"

#include "../Urho3D/IO/FileSystem.h"

#include "../Urho3D/Resource/ResourceCache.h"



using namespace Urho3D;

namespace tb
{


class TBFile
{
public:
    enum TBFileMode
    {
        MODE_READ
    };
    static TBFile* Open(const char* filename, TBFileMode mode);

    virtual ~TBFile() {}
    virtual long Size() = 0;
    virtual size_t Read(void* buf, size_t elemSize, size_t count) = 0;
};

class UTBFile : public TBFile
{
public:
    Urho3D::Context* context_;
    UTBFile(Urho3D::Context* _pContext)
        : ufileSize_(0)
    {
        context_ = _pContext;
    }

    virtual ~UTBFile()
    {
        if (pfile_)
        {
            pfile_->Close();
            pfile_ = NULL;
        }
    }

    bool OpenFile(const char* fileName)
    {

        Urho3D::ResourceCache* resourceCache = context_->GetSubsystem<Urho3D::ResourceCache>();

        pfile_ = resourceCache->GetFile(Urho3D::String(fileName));

        if (pfile_ == NULL)
            return false;

        bool bopen = pfile_->IsOpen();

        if (bopen)
        {
            ufileSize_ = pfile_->Seek((unsigned)-1);
            pfile_->Seek(0);
        }

        return bopen;
    }

    virtual long Size() { return (long)ufileSize_; }

    virtual size_t Read(void* buf, size_t elemSize, size_t count) { return pfile_->Read(buf, (int)(elemSize * count)); }

protected:
    Urho3D::SharedPtr<Urho3D::File> pfile_;
    unsigned ufileSize_;
};


URHO3D_API TBFile* TBFile::Open(const char* filename, TBFileMode)
{
  
    UTBFile* pFile = new UTBFile(Application::GetContext());
    if (!pFile->OpenFile(filename))
    {
        delete pFile;
        pFile = NULL;
    }
    return pFile;
}

}

URHO3D_API tb::TBFile* TBFileOpen(const char* filename, tb::TBFile::TBFileMode mode)
{
    // UTBFile* pFile;

    tb::UTBFile* pFile = new tb::UTBFile(Application::GetContext());
    if (!pFile->OpenFile(filename))
    {
        delete pFile;
        pFile = NULL;
    }
    return pFile;
}

