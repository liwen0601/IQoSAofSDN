
dpiDir=iqos/dpi
conDir=iqos/ryu-controller
netDir=network
toolDir=tool

#1. build dpi
cd $dpiDir
make clean && make
cd -

cp $dpiDir/dpiDeamon/dpiDeamon  $netDir/
cp $dpiDir/dpiDeamon/ip.properties $netDir/
cp $dpiDir/dpiClient/dpiClient  $netDir/

#2. build tools
cd tool
./build.sh
cd -

cp $toolDir/client $netDir/
cp $toolDir/server $netDir/

#3. copy controller
cp $conDir/* $netDir/

#4.finish
echo
echo "Build successfully, experiment could be done in directory network..."
echo 