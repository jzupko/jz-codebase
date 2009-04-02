//
// Copyright (c) 2009 Joseph A. Zupko
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 

#include <jz_core/Logger.h>
#include <jz_core/Memory.h>
#include <jz_graphics/Effect.h>
#include <jz_graphics/Graphics.h>
#include <jz_graphics/Material.h>
#include <jz_graphics/Texture.h>
#include <jz_system/Files.h>
#include <jz_system/ReadHelpers.h>
#include <jz_system/WriteHelpers.h>

namespace jz
{
    namespace graphics
    {

        enum ParameterType
        {
            kSingle = 1,
            kMatrix4 = 2,
            kVector2 = 3,
            kVector3 = 4,
            kVector4 = 5,
            kTexture = 6,
            kBoolean = 7
        };

        template <typename T>
        class MaterialParameter sealed : public IMaterialParameter
        {
        public:
            MaterialParameter(const string& aSemantic, const T& aValue)
                : mSemantic(aSemantic), mValue(aValue)
            {}

            ~MaterialParameter()
            {}

            string GetSemantic() const override { return mSemantic; }
            const T& GetValue() const { return mValue; }

            void Apply() override
            {
                mParam.Set(mValue);
            }

            void AttachTo(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                }
            }

            bool Validate(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                    return (mParam.IsValid());
                }

                return false;
            }
            
        private:
            MaterialParameter();
            MaterialParameter(const MaterialParameter&);
            MaterialParameter& operator=(const MaterialParameter&);

            Parameter<T> mParam;
            const string mSemantic;
            T mValue;
        };

        template <>
        class MaterialParameter<bool> sealed : public IMaterialParameter
        {
        public:
            MaterialParameter(const string& aSemantic, const bool& aValue)
                : mSemantic(aSemantic), mValue(aValue)
            {}

            ~MaterialParameter()
            {}

            string GetSemantic() const override { return mSemantic; }
            const bool& GetValue() const { return mValue; }

            void Apply() override
            {
                mParam.Set(mValue);
            }

            void AttachTo(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                }
            }

            bool Validate(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                    return (mParam.IsValid());
                }

                return false;
            }            

        private:
            MaterialParameter();
            MaterialParameter(const MaterialParameter&);
            MaterialParameter& operator=(const MaterialParameter&);

            Parameter<bool> mParam;
            const string mSemantic;
            bool mValue;
        };

        template <>
        class MaterialParameter<float> sealed : public IMaterialParameter
        {
        public:
            MaterialParameter(const string& aSemantic, const float& aValue)
                : mSemantic(aSemantic), mValue(aValue)
            {}

            ~MaterialParameter()
            {}

            string GetSemantic() const override { return mSemantic; }
            const float& GetValue() const { return mValue; }

            void Apply() override
            {
                mParam.Set(mValue);
            }

            void AttachTo(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                }
            }

            bool Validate(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                    return (mParam.IsValid());
                }

                return false;
            }             

        private:
            MaterialParameter();
            MaterialParameter(const MaterialParameter&);
            MaterialParameter& operator=(const MaterialParameter&);

            Parameter<float> mParam;
            const string mSemantic;
            float mValue;
        };

        template <>
        class MaterialParameter<Texture> sealed : public IMaterialParameter
        {
        public:
            MaterialParameter(const string& aSemantic, const TexturePtr& aValue)
                : mSemantic(aSemantic), mValue(aValue)
            {}

            ~MaterialParameter()
            {}

            string GetSemantic() const override { return mSemantic; }
            const TexturePtr& GetValue() const { return mValue; }

            void Apply() override
            {
                mParam.Set(mValue);
            }

            void AttachTo(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                }
            }

            bool Validate(Effect* p) override
            {
                if (p)
                {
                    p->GetParameterBySemantic(mSemantic, mParam);
                    return (mParam.IsValid());
                }

                return false;
            }             

        private:
            MaterialParameter();
            MaterialParameter(const MaterialParameter&);
            MaterialParameter& operator=(const MaterialParameter&);

            Parameter<Texture> mParam;
            const string mSemantic;
            TexturePtr mValue;
        };

        Material::Material(const string& aFilename)
            : IObject(aFilename), mbDirty(true)
        {}
        
        Material::~Material()
        {}

        void Material::AttachTo(Effect* p)
        {
            mpAttachedTo.Reset(p);
            if (IsLoaded())
            {
                _Attach();
            }
        }

        void Material::Apply() const
        {
            if (IsReset() && mpAttachedTo.IsValid())
            {
                for (vector<IMaterialParameter*>::const_iterator I = mParameters.begin(); I != mParameters.end(); I++)
                {
                    (*I)->Apply();
                }
            }
        }

        inline IObject::State SetLoaded()
        {
            if (Graphics::GetSingleton().IsLost()) { return (IObject::kLost); }
            else { return (IObject::kReset); }
        }

        void Material::AddParameter(const string& s, bool v)
        {
            mParameters.push_back(new MaterialParameter<bool>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, float v)
        {
            mParameters.push_back(new MaterialParameter<float>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, const Matrix4& v)
        {
            mParameters.push_back(new MaterialParameter<Matrix4>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, const Vector2& v)
        {
            mParameters.push_back(new MaterialParameter<Vector2>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, const Vector3& v)
        {
            mParameters.push_back(new MaterialParameter<Vector3>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, const Vector4& v)
        {
            mParameters.push_back(new MaterialParameter<Vector4>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::AddParameter(const string& s, Texture* v)
        {
            mParameters.push_back(new MaterialParameter<Texture>(s, v));
            mInternalState = SetLoaded();
            _Attach();
        }

        void Material::Save() const
        {
            using namespace system;
            if (!mbDirty) { return; }

            IWriteFilePtr pFile = Files::GetSingleton().OpenWriteable(GetFilename().c_str());

            int count = (int)(mParameters.size());

            WriteInt32(pFile, count);
            for (int i = 0; i < count; i++)
            {
                WriteString(pFile, mParameters[i]->GetSemantic());

                if (typeid(*(mParameters[i])) == typeid(MaterialParameter<float>))
                {
                    WriteInt32(pFile, kSingle);
                    WriteSingle(pFile, static_cast<MaterialParameter<float> const*>(mParameters[i])->GetValue());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<Matrix4>))
                {
                    WriteInt32(pFile, kMatrix4);
                    WriteMatrix4(pFile, static_cast<MaterialParameter<Matrix4> const*>(mParameters[i])->GetValue());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<Vector2>))
                {
                    WriteInt32(pFile, kVector2);
                    WriteVector2(pFile, static_cast<MaterialParameter<Vector2> const*>(mParameters[i])->GetValue());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<Vector3>))
                {
                    WriteInt32(pFile, kVector3);
                    WriteVector3(pFile, static_cast<MaterialParameter<Vector3> const*>(mParameters[i])->GetValue());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<Vector4>))
                {
                    WriteInt32(pFile, kVector4);
                    WriteVector4(pFile, static_cast<MaterialParameter<Vector4> const*>(mParameters[i])->GetValue());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<Texture>))
                {
                    WriteInt32(pFile, kTexture);
                    
                    TexturePtr pTexture = static_cast<MaterialParameter<Texture> const*>(mParameters[i])->GetValue();
                    WriteString(pFile, pTexture->GetFilename());
                }
                else if (typeid(*(mParameters[i])) == typeid(MaterialParameter<bool>))
                {
                    WriteInt32(pFile, kBoolean);
                    WriteBoolean(pFile, static_cast<MaterialParameter<bool> const*>(mParameters[i])->GetValue());
                }
            }

            const_cast<bool&>(mbDirty) = false;
        }

        static TexturePtr ReadTexture(system::IReadFilePtr& p)
        {
            string filename = system::ReadString(p);
            
            TexturePtr pRet(Graphics::GetSingleton().Create<Texture>(filename));

            return pRet;
        }

        void Material::_Attach()
        {
            if (mpAttachedTo.IsValid())
            {
                size_t count = mParameters.size();
                for (size_t i = 0; i < count; i++)
                {
                    bool b = (mParameters[i]->Validate(mpAttachedTo.Get()));
                
                    if (!b)
                    {
                        LogMessage("Failed attaching material \"" + GetFilename() + "\" to Effect \"" + mpAttachedTo->GetFilename() + "\".", Logger::kError);
                        LogMessage("Parameter with semantic \"" + mParameters[i]->GetSemantic() + "\" failed validation.", Logger::kError);
                    
                        _DestroyParameters();
                        mInternalState = (kErrorGraphics);
                        return;
                    }
                }

                for (size_t i = 0; i < count; i++)
                {
                    mParameters[i]->AttachTo(mpAttachedTo.Get());
                }
            }
        }

        void Material::_DestroyParameters()
        {
            size_t count = mParameters.size();
            for (size_t i = 0; i < count; i++) { delete mParameters[i]; }
            mParameters.clear();            
        }

        IObject::State Material::_Load()
        {
            using namespace system;
            mbDirty = true;

            IReadFilePtr pFile;
            try
            {
                pFile = Files::GetSingleton().Open(GetFilename().c_str());
            }
            catch (std::exception&)
            {
                return (kErrorFileNotFound);
            }

            try
            {
                string semantic;
                int parameterType;
                int count = ReadInt32(pFile);
                for (int i = 0; i < count; i++)
                {
                    semantic = ReadString(pFile);
                    parameterType = ReadInt32(pFile);
                    ParameterType type = (ParameterType)parameterType;

                    switch (type)
                    {
                    case kSingle: mParameters.push_back(new MaterialParameter<float>(semantic, ReadSingle(pFile))); break;
                    case kMatrix4: mParameters.push_back(new MaterialParameter<Matrix4>(semantic, ReadMatrix4(pFile))); break;
                    case kTexture: mParameters.push_back(new MaterialParameter<Texture>(semantic, ReadTexture(pFile))); break;
                    case kVector2: mParameters.push_back(new MaterialParameter<Vector2>(semantic, ReadVector2(pFile))); break;
                    case kVector3: mParameters.push_back(new MaterialParameter<Vector3>(semantic, ReadVector3(pFile))); break;
                    case kVector4: mParameters.push_back(new MaterialParameter<Vector4>(semantic, ReadVector4(pFile))); break;
                    case kBoolean: mParameters.push_back(new MaterialParameter<bool>(semantic, ReadBoolean(pFile))); break;
                    default:
                        JZ_E_ON_FAIL(false, "invalid texture parameters type.");
                    }
                }
            }
            catch (std::exception&)
            {
                _DestroyParameters();
                return (kErrorDataRead);
            }

            _Attach();

            return (kLost);
        }

        IObject::State Material::_Unload()
        {
            _DestroyParameters();

            return (kUnloaded);
        }

    }
}
