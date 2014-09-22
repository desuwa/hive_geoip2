# libmaxminddb Ruby bindings

### Installation

Install [libmaxminddb](https://github.com/maxmind/libmaxminddb), then:

```
gem install hive_geoip2
```

### Usage

```ruby
Hive::GeoIP2.lookup('88.174.0.1', 'GeoLite2-City.mmdb')
```

or

```ruby
mmdb = Hive::GeoIP2.new('GeoLite2-City.mmdb')
mmdb.lookup('88.174.0.1')
mmdb.close
```

The result is returned as a `Hash`:

```ruby
{
"city": {
  "geoname_id": 3006995,
  "names": {
    "en": "La Remuee"
  }
},
"continent": {
  "code": "EU",
  "geoname_id": 6255148,
  "names": {
    "de": "Europa",
    "en": "Europe",
    "es": "Europa",
    "fr": "Europe",
    "ja": "ヨーロッパ",
    "pt-BR": "Europa",
    "ru": "Европа",
    "zh-CN": "欧洲"
  }
},
"country": {
  "geoname_id": 3017382,
  "iso_code": "FR",
  "names": {
    "de": "Frankreich",
    "en": "France",
    "es": "Francia",
    "fr": "France",
    "ja": "フランス共和国",
    "pt-BR": "França",
    "ru": "Франция",
    "zh-CN": "法国"
  }
},
"location": {
  "latitude": 49.5277,
  "longitude": 0.4028,
  "time_zone": "Europe/Paris"
},
"registered_country": {
  "geoname_id": 3017382,
  "iso_code": "FR",
  "names": {
    "de": "Frankreich",
    "en": "France",
    "es": "Francia",
    "fr": "France",
    "ja": "フランス共和国",
    "pt-BR": "França",
    "ru": "Франция",
    "zh-CN": "法国"
  }
},
"subdivisions": [
  {
    "geoname_id": 3013756,
    "iso_code": "Q",
    "names": {
      "de": "Haute-Normandie",
      "en": "Upper Normandy",
      "es": "Alta Normandía",
      "fr": "Haute-Normandie",
      "ja": "オート＝ノルマンディー地域圏",
      "pt-BR": "Alta Normandia",
      "ru": "Верхняя Нормандия"
    }
  },
  {
    "geoname_id": 2975248,
    "iso_code": "76",
    "names": {
      "de": "Seine-Maritime",
      "en": "Seine-Maritime",
      "es": "Sena Marítimo",
      "fr": "Seine-Maritime",
      "pt-BR": "Sena Marítimo"
    }
  }
]
}
```
