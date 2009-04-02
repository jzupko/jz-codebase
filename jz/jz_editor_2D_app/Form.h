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

#pragma once
#ifndef _JZ_EDITOR_2D_APP_FORM_H_
#define _JZ_EDITOR_2D_APP_FORM_H_

#include <jz_core/Logger.h>
#include <jz_editor_2D/Editor.h>
#include <jz_system/Win32.h>

namespace jz
{
    namespace editor_2D_app
    {
        using namespace System;
        using namespace System::ComponentModel;
        using namespace System::Collections;
        using namespace System::Windows::Forms;
        using namespace System::Data;
        using namespace System::Drawing;

        public ref class Form : public System::Windows::Forms::Form
        {
        public:
            Form()
            {
                InitializeComponent();
            }

            void SetConsoleText(cli::array<String^, 1>^ aLines)
            {
                this->Console->Lines = aLines;
            }

        protected:
            ~Form()
            {
                if (mComponents) { delete mComponents; }
            }

        private:
            System::ComponentModel::Container^ mComponents;
        private: System::Windows::Forms::Button^  CreateSprite;
        private: System::Windows::Forms::Button^  DeleteSprite;
        private: System::Windows::Forms::Button^  CreateLight;

                 System::Windows::Forms::TextBox^  Console;

            void InitializeComponent()
            {
                this->Console = (gcnew System::Windows::Forms::TextBox());
                this->CreateSprite = (gcnew System::Windows::Forms::Button());
                this->DeleteSprite = (gcnew System::Windows::Forms::Button());
                this->CreateLight = (gcnew System::Windows::Forms::Button());
                this->SuspendLayout();
                // 
                // Console
                // 
                this->Console->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
                    | System::Windows::Forms::AnchorStyles::Right));
                this->Console->Location = System::Drawing::Point(12, 245);
                this->Console->Multiline = true;
                this->Console->Name = L"Console";
                this->Console->ReadOnly = true;
                this->Console->Size = System::Drawing::Size(376, 142);
                this->Console->TabIndex = 0;
                this->Console->WordWrap = false;
                // 
                // CreateSprite
                // 
                this->CreateSprite->Location = System::Drawing::Point(12, 11);
                this->CreateSprite->Name = L"CreateSprite";
                this->CreateSprite->Size = System::Drawing::Size(96, 30);
                this->CreateSprite->TabIndex = 1;
                this->CreateSprite->Text = L"Create Sprite";
                this->CreateSprite->UseVisualStyleBackColor = true;
                this->CreateSprite->Click += gcnew System::EventHandler(this, &Form::CreateSprite_Click);
                // 
                // DeleteSprite
                // 
                this->DeleteSprite->Location = System::Drawing::Point(114, 11);
                this->DeleteSprite->Name = L"DeleteSprite";
                this->DeleteSprite->Size = System::Drawing::Size(96, 30);
                this->DeleteSprite->TabIndex = 2;
                this->DeleteSprite->Text = L"Delete Sprite";
                this->DeleteSprite->UseVisualStyleBackColor = true;
                this->DeleteSprite->Click += gcnew System::EventHandler(this, &Form::DeleteSprite_Click);
                // 
                // CreateLight
                // 
                this->CreateLight->Location = System::Drawing::Point(12, 47);
                this->CreateLight->Name = L"CreateLight";
                this->CreateLight->Size = System::Drawing::Size(96, 30);
                this->CreateLight->TabIndex = 3;
                this->CreateLight->Text = L"Create Light";
                this->CreateLight->UseVisualStyleBackColor = true;
                this->CreateLight->Click += gcnew System::EventHandler(this, &Form::CreateLight_Click);
                // 
                // Form
                // 
                this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
                this->ClientSize = System::Drawing::Size(400, 400);
                this->Controls->Add(this->CreateLight);
                this->Controls->Add(this->DeleteSprite);
                this->Controls->Add(this->CreateSprite);
                this->Controls->Add(this->Console);
                this->Name = L"Form";
                this->Text = L"JZ Editor 2D";
                this->TopMost = true;
                this->Load += gcnew System::EventHandler(this, &Form::Form_Load);
                this->Closed += gcnew System::EventHandler(this, &Form::Form_Close);
                this->ResumeLayout(false);
                this->PerformLayout();

            }

        private:
            System::Void Form_Close(System::Object^ aSender, System::EventArgs^ e)
            {
                PostQuitMessage(0);
            }

            System::Void Form_Load(System::Object^ aSender, System::EventArgs^ e)
            {}
        
            System::Void CreateSprite_Click(System::Object^ aSender, System::EventArgs^ e)
            {
                jz::editor_2D::Editor& editor = jz::editor_2D::Editor::GetSingleton();

                editor.Call("jzCreateSprite");
            }
        
            System::Void DeleteSprite_Click(System::Object^ aSender, System::EventArgs^ e)
            {
                jz::editor_2D::Editor& editor = jz::editor_2D::Editor::GetSingleton();

                editor.Call("jzDeleteSprite");
            }
        
            System::Void CreateLight_Click(System::Object^ aSender, System::EventArgs^ e)
            {
                jz::editor_2D::Editor& editor = jz::editor_2D::Editor::GetSingleton();

                editor.Call("jzCreateLight");
            }
        };

    }
}

#endif
