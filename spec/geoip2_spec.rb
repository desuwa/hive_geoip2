#!/usr/bin/env ruby
# encoding: utf-8

require 'minitest/autorun'
require 'hive_geoip2'

include Hive

class GeoIP2Spec < MiniTest::Spec
  
  ROOT = File.expand_path(File.dirname(__FILE__)) + '/data'
  
  TYPES = {
    int32: -1 * (2**28),
    uint16: 100,
    uint32: 2**28,
    uint64: 1 << 60,
    uint128: 1 << 120,
    float: 1.1,
    double: 42.123456,
    boolean: true,
    utf8_string: 'unicode! ☯ - ♫',
    map: { 'key' => 'value' },
    array: [1, 2, 3],
    bytes: [42].pack('N'),
  }
  
  @@res = GeoIP2.lookup('88.174.0.0', "#{ROOT}/database.mmdb")
  
  describe GeoIP2 do
    it 'parses int32' do
      @@res['int32'].must_equal TYPES[:int32]
    end
    
    it 'parses uint16' do
      @@res['uint16'].must_equal TYPES[:uint16]
    end
    
    it 'parses uint32' do
      @@res['uint32'].must_equal TYPES[:uint32]
    end
    
    it 'parses uint64 as Bignum' do
      @@res['uint64'].must_equal TYPES[:uint64]
    end
    
    it 'parses uint128 as Bignum' do
      @@res['uint128'].must_equal TYPES[:uint128]
    end
    
    it 'parses floats' do
      @@res['float'].to_s[0, 3].must_equal TYPES[:float].to_s[0, 3]
    end
    
    it 'parses doubles' do
      @@res['double'].to_s[0, 9].must_equal TYPES[:double].to_s[0, 9]
    end
    
    it 'parses booleans' do
      @@res['boolean'].must_equal TYPES[:boolean]
    end
    
    it 'parses utf8 strings' do
      @@res['utf8_string'].encoding.must_equal Encoding::UTF_8
      @@res['utf8_string'].must_equal TYPES[:utf8_string]
    end
    
    it 'parses hashes' do
      @@res['map'].eql?(TYPES[:map]).must_equal true
    end
    
    it 'parses arrays' do
      @@res['array'].eql?(TYPES[:array]).must_equal true
    end
    
    it 'parses byte strings' do
      @@res['bytes'].encoding.must_equal Encoding::BINARY
      @@res['bytes'].must_equal TYPES[:bytes]
    end
    
    it 'returns nil if the IP was not found' do
      GeoIP2.lookup('127.0.0.1', "#{ROOT}/database.mmdb").must_be_nil
    end
    
    it 'can be instantiated' do
      geo = GeoIP2.new("#{ROOT}/database.mmdb")
      geo.lookup('88.174.0.0').must_be_instance_of(Hash)
      geo.close
    end
    
    it 'raises IOError when calling #lookup on a closed database' do
      -> {
        geo = GeoIP2.new("#{ROOT}/database.mmdb")
        geo.close
        geo.lookup('88.174.0.0')
      }.must_raise(IOError)
    end
    
    it 'raises IOError if the database cannot be opened' do
      -> {
        GeoIP2.new("#{ROOT}/nope.mmdb")
      }.must_raise(IOError)
    end
    
    describe '#closed?' do
      it 'returns false if the database is open' do
        assert GeoIP2.new("#{ROOT}/database.mmdb").closed? == false
      end
      
      it 'returns true if the database is closed' do
        geo = GeoIP2.new("#{ROOT}/database.mmdb")
        geo.close
        assert geo.closed? == true
      end
    end
 end
  
end
