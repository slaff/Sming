# Host build.run.sh


if [ "1" = "0" ];  then

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"

if [[ $CHECK_SCA -eq 1 ]]; then
	$DIR/coverity-scan.sh
else
	$MAKE_PARALLEL Basic_Blink Basic_DateTime Basic_Delegates Basic_Interrupts Basic_ProgMem Basic_Serial Basic_Servo Basic_Ssl LiveDebug DEBUG_VERBOSE_LEVEL=3
fi

# Build and run tests
export SMING_TARGET_OPTIONS='--flashfile=$(FLASH_BIN) --flashsize=$(SPI_SIZE)'
$MAKE_PARALLEL tests

fi


# Do integration test

echo "INTEGRATION TESTS";

# Check server headers
cd $SMING_PROJECTS_DIR/samples/HttpServer_Bootstrap
make WIFI_SSID=Dentist WIFI_PWD=fillings
make flash
nohup make run &

sleep 30 # wait for the HttpServer to start accepting connections...

cat nohup.out

RESPONSE_HEADERS=$(curl -s -D - -o /dev/null http://192.168.13.10/bootstrap.css)

echo "Got headers: [$RESPONSE_HEADERS]"

# Content-Type: text/css
if ! [[ $RESPONSE_HEADERS == *"Content-Length: "* ]]; then
  echo "Missing content length!"
  exit 1;
fi

# Content-Encoding: gzip
if ! [[ $RESPONSE_HEADERS == *"Content-Encoding: gzip"* ]]; then
  echo "Missing or Invalid Content-Encoding!"
  exit 1;
fi

# Content-Type: text/css
if ! [[ $RESPONSE_HEADERS == *"Content-Type: text/css"* ]]; then
  echo "Missing or Invalid Content-Type!"
  exit 1;
fi