# zsign-ios
This is an implementation package of [zsign](https://github.com/zhlynn/zsign) to sign apps for Apple Devices. This comes included with OpenSSL as a package.

## How To Install
In your Xcode project click file then Add Package Dependencies...
Click local and go to the downloaded folder from the zsign-ios repository.

## Usage
Package declaration goes as follows:
```swift
import ZSign
```

### Signing a folder
This zsign implementation requires and extracted IPA files payload folder to sign. Using the direct IPA file in the folderToSign variable will return an error.

> [!TIP]
> You can use third party packages to extract an IPA file into a folder for signing. I recommend using [ZipFoundation](https://github.com/weichsel/ZIPFoundation.git)

Due to iOS limitations you cant directly sign an IPA file so extracting it into a folder is the best and only available option here.

certPath is a variable for the cert file which is either in .cer or .pem format. This is not required if you're using .p12 and .mobileprovision certificate files. Though it isnt required you can leave this variable with the same file path as your .p12 file when calling the signing process.

p12KeyPath is self explanitory and is for the .p12 file of your certificate.
provisioningProfilePath is also self explanitory and is for the .mobileprovision file of your certificate.

```swift
let code = zsign(
    folderToSign,
    certPath,
    p12KeyPath,
    provisioningProfilePath,
    p12Password
)
```
Once the app is signed it will return a code which 0 means it signed correclty and returned no errors.

```swift
if code == 0 {
 // once completed
}
```

The signed payload folder is located in the same directory and is the same folder as your originally unsigned one. You can turn this into an IPA file with other zipping methods which I wont go into.
