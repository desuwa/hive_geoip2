Gem::Specification.new do |s|
  s.name = 'hive_geoip2'
  s.version = '0.1.1'
  s.summary = 'libmaxminddb GeoIP2 Ruby bindings'
  s.author = 'Maxime Youdine'
  s.license = 'MIT'
  s.homepage = 'https://github.com/desuwa/hive_geoip2'
  s.required_ruby_version = '>= 1.9.2'
  s.files = %w[
    hive_geoip2.gemspec
    lib/hive_geoip2/hive_geoip2.rb
    ext/hive_geoip2/extconf.rb
    ext/hive_geoip2/hive_geoip2.c
  ]
  s.extensions = ['ext/hive_geoip2/extconf.rb']
  s.require_paths = ['lib']
end
