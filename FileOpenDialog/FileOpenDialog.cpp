// C++
// x64

#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <wrl/client.h>
#include <ShObjIdl_core.h>
#include <comdef.h>

using namespace std;
using namespace Microsoft::WRL;

void HR(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}

int main()
{
    try
    {
        ComPtr<IFileOpenDialog> pFileDlg;
        ComPtr<IShellItem> pSelectedItem;

        HR(CoInitialize(nullptr));
        HR(CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, __uuidof(IFileOpenDialog),
            reinterpret_cast<void**>(pFileDlg.GetAddressOf())));

        if (SUCCEEDED(pFileDlg->Show(nullptr)))
        {
            HR(pFileDlg->GetResult(pSelectedItem.GetAddressOf()));

            wchar_t* pFileName = nullptr;
            wchar_t* pFilePath = nullptr;
            
            HANDLE hFile = INVALID_HANDLE_VALUE;
            LARGE_INTEGER fileSize = { 0 };

            HRESULT hr = pSelectedItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pFileName);
            
            if (SUCCEEDED(hr))
            {
                hr = pSelectedItem->GetDisplayName(SIGDN_FILESYSPATH, &pFilePath);
            }

            if (SUCCEEDED(hr))
            {
                hFile = CreateFile(pFilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            }

            if (hFile != INVALID_HANDLE_VALUE)
            {
                GetFileSizeEx(hFile, &fileSize);
            }

            wcout << fixed;
            wcout << setprecision(2);
            wcout << L"File name: " << pFileName << endl;
            wcout << L"File path: " << pFilePath << endl;
            wcout << L"File size (MB): " << fileSize.QuadPart / 1000000.0f << endl;
            
            CoTaskMemFree(pFileName);
            CoTaskMemFree(pFilePath);

            if (hFile != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hFile);
            }
        }
    }
    catch (_com_error err)
    {
        wcout << err.ErrorMessage() << endl;
        wcout << L"HRESULT ERROR CODE: " << hex << err.Error() << endl;
    }

    wcin.get();

    return 0;
}

