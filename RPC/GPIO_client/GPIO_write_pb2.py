# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: GPIO_write.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='GPIO_write.proto',
  package='',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=b'\n\x10GPIO_write.proto\"6\n\x0fModeInputParams\x12\x10\n\x08gpio_pin\x18\x01 \x01(\r\x12\x11\n\tgpio_mode\x18\x02 \x01(\r\"6\n\x0eSetInputParams\x12\x10\n\x08gpio_pin\x18\x01 \x01(\r\x12\x12\n\ngpio_level\x18\x02 \x01(\r\"\x07\n\x05\x45mpty2U\n\x07PI_GPIO\x12&\n\x08set_mode\x12\x10.ModeInputParams\x1a\x06.Empty\"\x00\x12\"\n\x05write\x12\x0f.SetInputParams\x1a\x06.Empty\"\x00\x62\x06proto3'
)




_MODEINPUTPARAMS = _descriptor.Descriptor(
  name='ModeInputParams',
  full_name='ModeInputParams',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='gpio_pin', full_name='ModeInputParams.gpio_pin', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='gpio_mode', full_name='ModeInputParams.gpio_mode', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=20,
  serialized_end=74,
)


_SETINPUTPARAMS = _descriptor.Descriptor(
  name='SetInputParams',
  full_name='SetInputParams',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='gpio_pin', full_name='SetInputParams.gpio_pin', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='gpio_level', full_name='SetInputParams.gpio_level', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=76,
  serialized_end=130,
)


_EMPTY = _descriptor.Descriptor(
  name='Empty',
  full_name='Empty',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=132,
  serialized_end=139,
)

DESCRIPTOR.message_types_by_name['ModeInputParams'] = _MODEINPUTPARAMS
DESCRIPTOR.message_types_by_name['SetInputParams'] = _SETINPUTPARAMS
DESCRIPTOR.message_types_by_name['Empty'] = _EMPTY
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

ModeInputParams = _reflection.GeneratedProtocolMessageType('ModeInputParams', (_message.Message,), {
  'DESCRIPTOR' : _MODEINPUTPARAMS,
  '__module__' : 'GPIO_write_pb2'
  # @@protoc_insertion_point(class_scope:ModeInputParams)
  })
_sym_db.RegisterMessage(ModeInputParams)

SetInputParams = _reflection.GeneratedProtocolMessageType('SetInputParams', (_message.Message,), {
  'DESCRIPTOR' : _SETINPUTPARAMS,
  '__module__' : 'GPIO_write_pb2'
  # @@protoc_insertion_point(class_scope:SetInputParams)
  })
_sym_db.RegisterMessage(SetInputParams)

Empty = _reflection.GeneratedProtocolMessageType('Empty', (_message.Message,), {
  'DESCRIPTOR' : _EMPTY,
  '__module__' : 'GPIO_write_pb2'
  # @@protoc_insertion_point(class_scope:Empty)
  })
_sym_db.RegisterMessage(Empty)



_PI_GPIO = _descriptor.ServiceDescriptor(
  name='PI_GPIO',
  full_name='PI_GPIO',
  file=DESCRIPTOR,
  index=0,
  serialized_options=None,
  serialized_start=141,
  serialized_end=226,
  methods=[
  _descriptor.MethodDescriptor(
    name='set_mode',
    full_name='PI_GPIO.set_mode',
    index=0,
    containing_service=None,
    input_type=_MODEINPUTPARAMS,
    output_type=_EMPTY,
    serialized_options=None,
  ),
  _descriptor.MethodDescriptor(
    name='write',
    full_name='PI_GPIO.write',
    index=1,
    containing_service=None,
    input_type=_SETINPUTPARAMS,
    output_type=_EMPTY,
    serialized_options=None,
  ),
])
_sym_db.RegisterServiceDescriptor(_PI_GPIO)

DESCRIPTOR.services_by_name['PI_GPIO'] = _PI_GPIO

# @@protoc_insertion_point(module_scope)
