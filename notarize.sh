#!/bin/sh  

if [ "$EUID" -ne 0 ]
  then echo "Error: Please run this command as root using sudo..."
  echo ""
  exit
fi	

PLUGIN_NAME="EXAMPLE"
DEVELOPER_ID="USER NAME (XXXXXXXXXX)"
APP_PASS="xxxx-xxxx-xxxx-xxxx"
APPLE_ID="email@icloud.com"
VERSION="1.0"
VERSION_UNDERSCORES="1_0"

if ! [ -d "tmp" ]; then
  `mkdir -p tmp`
fi

if ! [ -d "install/resources" ]; then
  `mkdir -p install/resources`
fi

UPPER=$(echo "$PLUGIN_NAME" |  tr '[:lower:]' '[:upper:]' )
BUILD_FOLDER=build/"$UPPER"_artefacts/Release

cp -r "$BUILD_FOLDER"/AU/"$PLUGIN_NAME".component tmp/
cp -r "$BUILD_FOLDER"/VST3/"$PLUGIN_NAME".vst3 tmp/
cp EULA.txt install/resources
cp README.txt install/resources
cp Welcome.txt install/resources
cp resources/img/BG.png install/resources


AU_PATH=tmp/"$PLUGIN_NAME".component
VST3_PATH=tmp/"$PLUGIN_NAME".vst3

#use pkgbuild to build the  plugin installers - and sign with Apple Developer ID Application certificate
sudo codesign -s "Developer ID Application: $DEVELOPER_ID" "$AU_PATH" --timestamp --force --deep -o runtime,hard
pkgbuild --install-location /Library/Audio/Plug-Ins/Components --identifier "$IDENTIFIER"au --version $VERSION --component "$AU_PATH" install/"$PLUGIN_NAME"_au.pkg 

sudo codesign -s "Developer ID Application: $DEVELOPER_ID" "$VST3_PATH" --timestamp --force --deep -o runtime,hard
pkgbuild --install-location /Library/Audio/Plug-Ins/VST3 --identifier "$IDENTIFIER"vst3 --version $VERSION --component "$VST3_PATH" install/"$PLUGIN_NAME"_vst3.pkg 

cd install

# use productbuild to synthesize installers
productbuild --synthesize --package "$PLUGIN_NAME"_au.pkg --package "$PLUGIN_NAME"_vst3.pkg distribution.xml

# include EULA, README, Welcome file, and Background image in installer
gsed -i~ '3i\    <license file="EULA.txt" mime-type="text/plain" />' distribution.xml
gsed -i~ '3i\    <welcome file="Welcome.txt" mime-type="text/plain" />' distribution.xml
gsed -i~ '3i\    <readme file="README.txt" mime-type="text/plain" />' distribution.xml
gsed -i~ '3i\    <background file="BG.png" alignment="bottomleft" mime-type="image/png" scaling="tofit" />' distribution.xml
gsed -i~ "3i\    <title>$PLUGIN_NAME</title>" distribution.xml

# use productbuild to combine everything into a single installer and sign with Apple Developer ID Installer certificate
productbuild --distribution distribution.xml --resources resources/ "$PLUGIN_NAME".pkg
productsign --sign "Developer ID Installer: $DEVELOPER_ID" "$PLUGIN_NAME".pkg "$PLUGIN_NAME"_MacOS_v_"$VERSION_UNDERSCORES".pkg

# Submit the final installer package for notarization
xcrun notarytool submit --wait --apple-id "$APPLE_ID" --password "$APP_PASS" --team-id "$TEAM_ID" "${PLUGIN_NAME}_MacOS_v_${VERSION_UNDERSCORES}.pkg"
xcrun stapler staple "${PLUGIN_NAME}_MacOS_v_${VERSION_UNDERSCORES}.pkg"

# Verify that the package works
spctl --verbose --assess --type install "${PLUGIN_NAME}_MacOS_v_${VERSION_UNDERSCORES}.pkg"


cd ..
rm -r tmp
echo "Done!"