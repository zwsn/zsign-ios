// swift-tools-version:5.3

import PackageDescription

let package = Package(
    name: "ZSignApple",
    platforms: [
        .iOS(.v12),
        .macOS(.v10_13),
    ],
    products: [
        .library(
            name: "ZSignApple",
            targets: ["ZSign"]),
    ],
    targets: [
        .target(
            name: "ZSign",
            dependencies: ["OpenSSL"],
            publicHeadersPath: "./Includes",
            cxxSettings: [
                .headerSearchPath("."),
            ],
            linkerSettings: [
                .linkedFramework("OpenSSL"),
            ]
        ),
        .binaryTarget(
            name: "OpenSSL",
            path: "Binaries/OpenSSL.xcframework"
        ),
    ],
    cxxLanguageStandard: .cxx14
)
