#include <windows.h>
#include <wuapi.h>
#include <wrl/client.h>
#include <iostream>
#include <comdef.h>

using namespace Microsoft::WRL;

void InitializeCOM() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library: " << std::hex << hr << std::endl;
        exit(hr);
    }
}

void UninitializeCOM() {
    CoUninitialize();
}

void CheckForUpdates() {
    HRESULT hr;

    // Initialize COM
    InitializeCOM();

    // Create an UpdateSession object
    ComPtr<IUpdateSession> updateSession;
    hr = CoCreateInstance(__uuidof(UpdateSession), nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&updateSession));
    if (FAILED(hr)) {
        std::cerr << "Failed to create UpdateSession: " << std::hex << hr << std::endl;
        UninitializeCOM();
        exit(hr);
    }

    // Create an UpdateSearcher object
    ComPtr<IUpdateSearcher> updateSearcher;
    hr = updateSession->CreateUpdateSearcher(&updateSearcher);
    if (FAILED(hr)) {
        std::cerr << "Failed to create UpdateSearcher: " << std::hex << hr << std::endl;
        UninitializeCOM();
        exit(hr);
    }

    // Search for updates
    BSTR criteria = SysAllocString(L"IsInstalled=0");
    ComPtr<ISearchResult> searchResult;
    hr = updateSearcher->Search(criteria, &searchResult);
    SysFreeString(criteria);
    if (FAILED(hr)) {
        std::cerr << "Failed to search for updates: " << std::hex << hr << std::endl;
        UninitializeCOM();
        exit(hr);
    }

    // Get the list of updates
    ComPtr<IUpdateCollection> updateCollection;
    hr = searchResult->get_Updates(&updateCollection);
    if (FAILED(hr)) {
        std::cerr << "Failed to get updates collection: " << std::hex << hr << std::endl;
        UninitializeCOM();
        exit(hr);
    }

    // Get the count of updates
    LONG updateCount = 0;
    hr = updateCollection->get_Count(&updateCount);
    if (FAILED(hr)) {
        std::cerr << "Failed to get update count: " << std::hex << hr << std::endl;
        UninitializeCOM();
        exit(hr);
    }

    std::wcout << L"Number of updates found: " << updateCount << std::endl;

    // Iterate through updates
    for (LONG i = 0; i < updateCount; ++i) {
        ComPtr<IUpdate> update;
        hr = updateCollection->get_Item(i, &update);
        if (FAILED(hr)) {
            std::cerr << "Failed to get update item: " << std::hex << hr << std::endl;
            continue;
        }

        BSTR title;
        hr = update->get_Title(&title);
        if (SUCCEEDED(hr)) {
            std::wcout << L"Update " << (i + 1) << L": " << title << std::endl;
            SysFreeString(title);
        }
    }

    // Uninitialize COM
    UninitializeCOM();
}

int main() {
    CheckForUpdates();
    return 0;
}
