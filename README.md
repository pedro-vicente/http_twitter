# Twitter query using C++

Weeekend project

# usage

get a developer account from Twitter 

https://developer.twitter.com/en/docs/tutorials/step-by-step-guide-to-making-your-first-request-to-the-twitter-api-v2

replace the string "my_secret_token" in the source code for the BEARER_TOKEN string obtained from Twitter

```
const std::string BEARER_TOKEN("my_secret_token");

```

optional: replace the input query in the source code with a supplied query as in

https://developer.twitter.com/en/docs/twitter-api/tweets/search/integrate/build-a-query

```
from:watch_econ -is:retweet
```

# build

Build on a terminal with

```
cmake -S . -B build
cd build
cmake --build . --parallel 9 
```


## dependencies

Cmake
OpenSSL
asio
nlohmann/json

### Ubuntu dependencies

```
sudo apt update
sudo apt-get install nlohmann-json3-dev
sudo apt-get install libssl-dev
sudo apt-get install cmake
sudo apt-get install libasio-dev
```

### MacOS dependencies

```
brew install nlohmann-json
```


input encoded

```
from%3Awatch_econ%20-is%3Aretweet
```


HTTP string

```
GET https://api.twitter.com/2/tweets/search/recent?query=from%3Awatch_econ%20-is%3Aretweet HTTP/1.1
Host: api.twitter.com
Authorization: Bearer MY_BEARER_STRING
Content-Type: application/json
Connection: close
```

header response

```
HTTP/1.1 200 OK
```

JSON response

```
{"data":[{"id":"1547434606059819009","text":"@aaronjmate @jaketapper Tapper is not the curious kind of journalist https://t.co/DPALnm8UV8"}],"meta":{"newest_id":"1547434606059819009","oldest_id":"1547434606059819009","result_count":1}}
id: 1547434606059819009
text: @aaronjmate @jaketapper Tapper is not the curious kind of journalist https://t.co/DPALnm8UV8
```
