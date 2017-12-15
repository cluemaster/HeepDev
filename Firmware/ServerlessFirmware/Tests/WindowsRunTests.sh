rm TestFirmwareIndexing.app
rm TestFirmwareUnIndexed.app
rm CTest/TestC.app

mingw32-make all

echo " "
echo " "
echo " "
echo " "
echo "Run UnIndexed Code"
echo " "
./TestFirmwareUnIndexed.app

echo " "
echo " "
echo " "
echo " "
echo "Run Indexed Code"
echo " "
./TestFirmwareIndexing.app

echo " "
echo " "
echo " "
echo " "
echo "Test C Complilation"
echo " "
cd CTest
mingw32-make
./TestC.app