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

#include "Form.h"
#include <jz_core/StringUtility.h>
#include <jz_editor_2D/Editor.h>
#include <jz_engine_2D/DrawMan.h>
#include <jz_graphics/Graphics.h>
#include <jz_system/Files.h>
#include <jz_system/System.h>
#include <jz_system/Time.h>

#if JZ_PLATFORM_WINDOWS
#   include <jz_system/Win32.h>
#   ifndef NDEBUG
#       include <crtdbg.h>
#   endif

int WINAPI WinMain(HINSTANCE aWindowInstance, HINSTANCE, LPSTR, int)
{
#   ifndef NDEBUG
        _CrtSetDbgFlag(_CRTDBG_CHECK_EVERY_1024_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#   endif

    jz::editor_2D_app::Application::EnableVisualStyles();
    jz::editor_2D_app::Application::SetCompatibleTextRenderingDefault(false); 

    try
    {
        jz::system::System system;

        std::string origDir = jz::system::Files::GetWorkingDirectory();
        std::string modulePath = jz::system::Files::ExtractPath(jz::system::Files::GetCurrentProcessFilename());

        jz::Scoped workingDir(
            std::tr1::bind(jz::system::Files::SetWorkingDirectory, modulePath),
            std::tr1::bind(jz::system::Files::SetWorkingDirectory, origDir));
     
        jz::system::Files::GetSingleton().AddArchive(new jz::system::FileArchive("..\\media\\compiled"));

        jz::graphics::Graphics graphics;
        jz::engine_2D::DrawMan dm;
        
        jz::editor_2D::Editor editor;

        jz::editor_2D_app::Form^ form = gcnew jz::editor_2D_app::Form();
        form->Show();

        MSG msg; 
        memset(&msg, 0, sizeof(MSG));
        while (msg.message != WM_QUIT)
        {
            if (PeekMessage(&msg, jz::null, 0u, 0u, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                jz::system::Time::GetSingleton().Tick();
                if (jz::Logger::IsLogBufferDirty())
                {
                    std::vector<std::string> buffer;
                    jz::Logger::GetLogBuffer(buffer);

                    const size_t kSize = buffer.size();
                    cli::array<System::String^, 1>^ lines = gcnew cli::array<System::String^, 1>(buffer.size());
                    for (size_t i = 0u; i < kSize; i++)
                    {
                        std::wstring wstr = jz::StringUtility::Convert(buffer[i]);
                        lines[i] = gcnew System::String(wstr.c_str());
                    }
                    form->SetConsoleText(lines);
                }
                editor.Update();
                editor.Pose();
                dm.Draw();
            }
        }       
    }
    catch (std::exception& e)
    {
        jz::LogMessage(e.what(), jz::Logger::kError);
        MessageBox(jz::null,
            e.what(),
            "Exception",
            MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 1;
    };

    return 0;
}

#endif
