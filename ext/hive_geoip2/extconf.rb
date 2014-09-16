require 'mkmf'

dir_config('maxminddb')

unless have_header('maxminddb.h') && have_library('maxminddb')
  abort 'Missing libmaxminddb'
end

create_makefile('hive_geoip2')
