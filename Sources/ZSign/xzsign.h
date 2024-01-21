//
//  zsign.h
//  ZSign-iOS
//
//  Created by loyahdev on 12/8/2023.
//

#ifndef zsign_h
#define zsign_h

#ifdef __cplusplus
extern "C"
{
#endif

    int zsign(const char *path,
              const char *certFile,
              const char *pKeyFile,
              const char *provFile,
              const char *password,
              const char *bundleId,
              const char *displayName);

#ifdef __cplusplus
}
#endif

#endif /* zsign_h */
