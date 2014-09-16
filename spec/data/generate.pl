#!/usr/bin/env perl

use utf8;

use Math::Int128 qw(uint128);
use MaxMind::DB::Writer::Tree;
use Net::Works::Network;

my %type_map = (
  int32 => 'int32',
  uint16 => 'uint16',
  uint32 => 'uint32',
  uint64 => 'uint64',
  uint128 => 'uint128',
  float => 'float',
  double => 'double',
  boolean => 'boolean',
  utf8_string => 'utf8_string',
  map => 'map',
  array => [ 'array', 'int32' ],
  bytes => 'bytes',
);

my $type_cb = sub { $type_map{$_[0]} // 'utf8_string' };

my $tree = MaxMind::DB::Writer::Tree->new(
  ip_version            => 4,
  record_size           => 32,
  database_type         => 'Data-Types',
  languages             => ['en'],
  description           => { en => 'Testing data types' },
  map_key_type_callback => $type_cb,
);

my $network = Net::Works::Network->new_from_string(string => '88.174.0.0/16');

$tree->insert_network(
  $network,
  {
    int32 => -1 * (2**28),
    uint16 => 100,
    uint32 => 2**28,
    uint64 => uint128(1) << 60,
    uint128 => uint128(1) << 120,
    float => 1.1,
    double => 42.123456,
    boolean => 1,
    utf8_string => 'unicode! ☯ - ♫',
    map => { key => 'value' },
    array => [1, 2, 3],
    bytes => pack('N', 42),
  },
);

open my $fh, '>:raw', 'database.mmdb';

$tree->write_tree($fh);
