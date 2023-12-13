#include "xzsign.h"
#include "common/common.h"
#include "common/json.h"
#include "openssl.h"
#include "macho.h"
#include "bundle.h"
#include <libgen.h>
#include <dirent.h>
#include <getopt.h>

int zsign(const char *path,
          
          const char *certFile,
          const char *pKeyFile,
          const char *provFile,
          const char *password
		  const char *bundleId
		  const char *displayName)
{
    bool bWeakInject = false;
    bool bForce = true;
    bool bEnableCache = false;
    
    string strCertFile = certFile ? certFile : "";
    string strPKeyFile = pKeyFile ? pKeyFile : "";
    string strProvFile = provFile ? provFile : "";
    string strPassword = password ? password : "";
    string strBundleId = bundleId ? bundleId : "";
    string strDisplayName = displayName ? displayName : "";
    string strBundleVersion;
    string strDyLibFile;
    string strEntitlementsFile;
    
    cout << strPKeyFile << endl;
    
    string strPath = path ? GetCanonicalizePath(path) : "";
    if (!IsFileExists(strPath.c_str()))
    {
        ZLog::ErrorV(">>> Invalid Path! %s\n", strPath.c_str());
        return -1;
    }
    
    ZSignAsset zSignAsset;
    if (!zSignAsset.Init(strCertFile, strPKeyFile, strProvFile, strEntitlementsFile, strPassword))
    {
        return -1;
    }
    
    string strFolder = strPath;
    ZAppBundle bundle;
    bool bRet = bundle.SignFolder(&zSignAsset, strFolder, strBundleId, strBundleVersion, strDisplayName, strDyLibFile, bForce, bWeakInject, bEnableCache);
    
    return bRet ? 0 : -1;
}

/*int zsign(const char *path,
          
          const char *certFile,
          const char *pKeyFile,
          const char *provFile,
          const char *password,
          const char *bundleId,
          const char *bundleVersion,
          const char *dyLibFile,
          const char *outputFile,
          const char *displayName,
          const char *entitlementsFile,

          uint8_t force,
          uint8_t install,
          uint8_t weakInject,
          uint32_t zipLevel)
{
	ZTimer gtimer;
    
    bool bWeakInject = weakInject > 0;
    bool bInstall = install > 0;
    bool bForce = force > 0;
    uint32_t uZipLevel = zipLevel;

    string strCertFile = certFile ? certFile : "";
    string strPKeyFile = pKeyFile ? pKeyFile : "";
    string strProvFile = provFile ? provFile : "";
    string strPassword = password ? password : "";
    string strBundleId = bundleId ? bundleId : "";
    string strBundleVersion = bundleVersion ? bundleVersion : "";
    string strDyLibFile = dyLibFile ? dyLibFile : "";
    string strOutputFile = outputFile ? GetCanonicalizePath(outputFile) : "";
    string strDisplayName = displayName ? displayName : "";
    string strEntitlementsFile = entitlementsFile ? entitlementsFile : "";

    
	string strPath = path ? GetCanonicalizePath(path) : "";
	if (!IsFileExists(strPath.c_str()))
	{
		ZLog::ErrorV(">>> Invalid Path! %s\n", strPath.c_str());
		return -1;
	}

	bool bZipFile = false;
	if (!IsFolder(strPath.c_str()))
	{
		bZipFile = IsZipFile(strPath.c_str());
		if (!bZipFile)
		{ //macho file
			ZMachO macho;
			if (macho.Init(strPath.c_str()))
			{
				if (!strDyLibFile.empty())
				{ //inject dylib
					bool bCreate = false;
					macho.InjectDyLib(bWeakInject, strDyLibFile.c_str(), bCreate);
				}
				else
				{
					macho.PrintInfo();
				}
				macho.Free();
			}
			return 0;
		}
	}

	ZTimer timer;
	ZSignAsset zSignAsset;
	if (!zSignAsset.Init(strCertFile, strPKeyFile, strProvFile, strEntitlementsFile, strPassword))
	{
		return -1;
	}

	bool bEnableCache = true;
	string strFolder = strPath;
	if (bZipFile)
	{ //ipa file
		bForce = true;
		bEnableCache = false;
		StringFormat(strFolder, "/tmp/zsign_folder_%llu", timer.Reset());
		ZLog::PrintV(">>> Unzip:\t%s (%s) -> %s ... \n", strPath.c_str(), GetFileSizeString(strPath.c_str()).c_str(), strFolder.c_str());
		RemoveFolder(strFolder.c_str());
        
		if (!UnzipFile(strFolder.c_str(), strPath.c_str()))
		{
			RemoveFolder(strFolder.c_str());
			ZLog::ErrorV(">>> Unzip Failed!\n");
			return -1;
		}
		timer.PrintResult(true, ">>> Unzip OK!");
	}

	timer.Reset();
	ZAppBundle bundle;
	bool bRet = bundle.SignFolder(&zSignAsset, strFolder, strBundleId, strBundleVersion, strDisplayName, strDyLibFile, bForce, bWeakInject, bEnableCache);
	timer.PrintResult(bRet, ">>> Signed %s!", bRet ? "OK" : "Failed");

	if (bInstall && strOutputFile.empty())
	{
		StringFormat(strOutputFile, "/tmp/zsign_temp_%llu.ipa", GetMicroSecond());
	}

	if (!strOutputFile.empty())
	{
		timer.Reset();
		size_t pos = bundle.m_strAppFolder.rfind("/Payload");
		if (string::npos == pos)
		{
			ZLog::Error(">>> Can't Find Payload Directory!\n");
			return -1;
		}

		ZLog::PrintV(">>> Archiving: \t%s ... \n", strOutputFile.c_str());
		string strBaseFolder = bundle.m_strAppFolder.substr(0, pos);
		char szOldFolder[PATH_MAX] = {0};
		if (NULL != getcwd(szOldFolder, PATH_MAX))
		{
			if (0 == chdir(strBaseFolder.c_str()))
			{
				uZipLevel = uZipLevel > 9 ? 9 : uZipLevel;
				RemoveFile(strOutputFile.c_str());
                ZipFile("Payload", strOutputFile.c_str(), uZipLevel);
				//SystemExec("zip -q -%u -r '%s' Payload", uZipLevel, strOutputFile.c_str());
				chdir(szOldFolder);
				if (!IsFileExists(strOutputFile.c_str()))
				{
					ZLog::Error(">>> Archive Failed!\n");
					return -1;
				}
			}
		}
		timer.PrintResult(true, ">>> Archive OK! (%s)", GetFileSizeString(strOutputFile.c_str()).c_str());
	}

	if (bRet && bInstall)
	{
        ZLog::Error(">>> ideviceinstaller not available for iOS !\n");
		//SystemExec("ideviceinstaller -i '%s'", strOutputFile.c_str());
	}

	if (0 == strOutputFile.find("/tmp/zsign_tmp_"))
	{
		RemoveFile(strOutputFile.c_str());
	}

	if (0 == strFolder.find("/tmp/zsign_folder_"))
	{
		RemoveFolder(strFolder.c_str());
	}

	gtimer.Print(">>> Done.");
	return bRet ? 0 : -1;
}*/
