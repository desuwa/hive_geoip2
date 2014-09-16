#define RSTRING_NOT_MODIFIED

#include <ruby/ruby.h>
#include <ruby/encoding.h>

#include "maxminddb.h"

#define WORDS_64 (64 / CHAR_BIT / SIZEOF_LONG)
#define WORDS_128 (128/ CHAR_BIT / SIZEOF_LONG)

VALUE rb_mHive;
VALUE rb_cGeoIP2;

static VALUE parse_data_list(MMDB_entry_data_list_s *data_list, VALUE *ret_obj);

struct args_parse_data_list {
  MMDB_entry_data_list_s *data_list;
  VALUE *ret_obj;
};

static VALUE guard_parse_data_list(VALUE arg) {
  struct args_parse_data_list *args = (struct args_parse_data_list *)arg;
  
  parse_data_list(args->data_list, args->ret_obj);
}

static VALUE parse_data_list(MMDB_entry_data_list_s *data_list, VALUE *ret_obj) {
  switch (data_list->entry_data.type) {
    case MMDB_DATA_TYPE_MAP:
    {
      uint32_t size = data_list->entry_data.data_size;
      
      VALUE val;
      
      VALUE hash = rb_hash_new();
      
      for (data_list = data_list->next; size && data_list; size--) {
        VALUE key = rb_enc_str_new(
          data_list->entry_data.utf8_string,
          data_list->entry_data.data_size,
          rb_utf8_encoding()
        );
        
        data_list = data_list->next;
        data_list = parse_data_list(data_list, &val);
        
        rb_hash_aset(hash, key, val);
        
      }
      
      *ret_obj = hash;
      
      break;
    }
    
    case MMDB_DATA_TYPE_ARRAY:
    {
      uint32_t size = data_list->entry_data.data_size;
      
      VALUE val;
      
      VALUE ary = rb_ary_new();
      
      for (data_list = data_list->next; size && data_list; size--) {
        data_list = parse_data_list(data_list, &val);
        rb_ary_push(ary, val);
      }
      
      *ret_obj = ary;
      
      break;
    }
    
    case MMDB_DATA_TYPE_UTF8_STRING:
    {
      *ret_obj = rb_enc_str_new(
        data_list->entry_data.utf8_string,
        data_list->entry_data.data_size,
        rb_utf8_encoding()
      );
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_BYTES:
    {
      *ret_obj = rb_enc_str_new(
        data_list->entry_data.bytes,
        data_list->entry_data.data_size,
        rb_ascii8bit_encoding()
      );
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_DOUBLE:
    {
      *ret_obj = rb_float_new(data_list->entry_data.double_value);
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_FLOAT:
    {
      *ret_obj = rb_float_new(data_list->entry_data.float_value);
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_UINT16:
    {
      *ret_obj = UINT2NUM(data_list->entry_data.uint16);
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_UINT32:
    {
      *ret_obj = UINT2NUM(data_list->entry_data.uint32);
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_BOOLEAN:
    {
      *ret_obj = data_list->entry_data.boolean ? Qtrue : Qfalse;
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_UINT64:
    {
      *ret_obj = rb_big_unpack(&data_list->entry_data.uint64, WORDS_64);
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_UINT128:
    {
#if MMDB_UINT128_IS_BYTE_ARRAY
      char buf[16];
      uint8_t i;
      uint8_t idx = 15;
      uint8_t *data = data_list->entry_data.uint128;
      
      for (i = 0; i < 16; ++i) {
        buf[i] = data[idx];
        idx--;
      }
      
      *ret_obj = rb_big_unpack(&buf, WORDS_128);
#else
      *ret_obj = rb_big_unpack(&data_list->entry_data.uint128, WORDS_128);
#endif
      
      data_list = data_list->next;
      
      break;
    }
    
    case MMDB_DATA_TYPE_INT32:
    {
      *ret_obj = INT2NUM(data_list->entry_data.int32);
      
      data_list = data_list->next;
      
      break;
    }
    
    default:
      rb_raise(rb_eRuntimeError,
        "GeoIP2 - %s", MMDB_strerror(MMDB_INVALID_DATA_ERROR)
      );
  }
  
  return data_list;
}

static VALUE rb_hive_geo_lookup(VALUE self, VALUE ip_arg, VALUE db_arg) {
  Check_Type(ip_arg, T_STRING);
  Check_Type(db_arg, T_STRING);
  
  char *ip_addr = StringValuePtr(ip_arg);
  char *db_path = StringValuePtr(db_arg);
  
  MMDB_s mmdb;
  
  int status = MMDB_open(db_path, MMDB_MODE_MMAP, &mmdb);
  
  if (status != MMDB_SUCCESS) {
    rb_raise(rb_eIOError, "GeoIP2 - %s: %s",
      MMDB_strerror(status), db_path
    );
  }
  
  int gai_error, mmdb_error;
  
  MMDB_lookup_result_s result =
    MMDB_lookup_string(&mmdb, ip_addr, &gai_error, &mmdb_error);
  
  if (mmdb_error != MMDB_SUCCESS) {
    MMDB_close(&mmdb);
    rb_raise(rb_eRuntimeError,
      "GeoIP2 - lookup failed: %s", MMDB_strerror(mmdb_error)
    );
  }
  
  if (gai_error != 0) {
    MMDB_close(&mmdb);
    rb_raise(rb_eRuntimeError,
      "GeoIP2 - getaddrinfo failed: %s", gai_strerror(gai_error)
    );
  }
  
  if (result.found_entry) {
    MMDB_entry_data_list_s *data_list, *first;
    
    status = MMDB_get_entry_data_list(&result.entry, &data_list);
    
    if (status != MMDB_SUCCESS) {
      MMDB_free_entry_data_list(data_list);
      MMDB_close(&mmdb);
      rb_raise(rb_eRuntimeError,
        "GeoIP2 - couldn\'t fetch results: %s", MMDB_strerror(status)
      );
    }
    
    first = data_list;
    
    int exception = 0;
    
    VALUE ret_obj;
    
    struct args_parse_data_list args;
    args.data_list = data_list;
    args.ret_obj = &ret_obj;
    
    rb_protect(guard_parse_data_list, (VALUE)&args, &exception);
    
    MMDB_free_entry_data_list(first);
    
    MMDB_close(&mmdb);
    
    if (exception) {
      rb_jump_tag(exception);
    }
    
    return ret_obj;
  }
  else {
    return Qnil;
  }
}

void Init_hive_geoip2() {
  rb_mHive = rb_define_module("Hive");
  rb_cGeoIP2 = rb_define_class_under(rb_mHive, "GeoIP2", rb_cObject);
  rb_define_singleton_method(rb_cGeoIP2, "lookup", rb_hive_geo_lookup, 2);
}
