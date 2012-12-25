/***************************************************************************************************
 *  Copyright 2012 MaidSafe.net limited                                                            *
 *                                                                                                 *
 *  The following source code is property of MaidSafe.net limited and is not meant for external    *
 *  use.  The use of this code is governed by the licence file licence.txt found in the root of    *
 *  this directory and also on www.maidsafe.net.                                                   *
 *                                                                                                 *
 *  You are not free to copy, amend or otherwise use this source code without the explicit         *
 *  written permission of the board of directors of MaidSafe.net.                                  *
 **************************************************************************************************/

#include "maidsafe/nfs/message.h"

#include "maidsafe/common/error.h"

#include "maidsafe/nfs/message_pb.h"


namespace maidsafe {

namespace nfs {

Message::Message(ActionType action_type,
                 PersonaType destination_persona_type,
                 PersonaType source_persona_type,
                 int data_type,
                 const NodeId& destination,
                 const NodeId& source,
                 const NonEmptyString& content,
                 const asymm::Signature& signature)
    : action_type_(action_type),
      destination_persona_type_(destination_persona_type),
      source_persona_type_(source_persona_type),
      data_type_(data_type),
      destination_(destination),
      source_(source),
      content_(content),
      signature_(signature) {
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

Message::Message(const Message& other)
    : action_type_(other.action_type_),
      destination_persona_type_(other.destination_persona_type_),
      source_persona_type_(other.source_persona_type_),
      data_type_(other.data_type_),
      destination_(other.destination_),
      source_(other.source_),
      content_(other.content_),
      signature_(other.signature_) {}

Message& Message::operator=(const Message& other) {
  action_type_ = other.action_type_;
  destination_persona_type_ = other.destination_persona_type_;
  source_persona_type_ = other.source_persona_type_;
  data_type_ = other.data_type_;
  destination_ = other.destination_;
  source_ = other.source_;
  content_ = other.content_;
  signature_ = other.signature_;
  return *this;
}

Message::Message(Message&& other)
    : action_type_(std::move(other.action_type_)),
      destination_persona_type_(std::move(other.destination_persona_type_)),
      source_persona_type_(std::move(other.source_persona_type_)),
      data_type_(std::move(other.data_type_)),
      destination_(std::move(other.destination_)),
      source_(std::move(other.source_)),
      content_(std::move(other.content_)),
      signature_(std::move(other.signature_)) {}

Message& Message::operator=(Message&& other) {
  action_type_ = std::move(other.action_type_);
  destination_persona_type_ = std::move(other.destination_persona_type_);
  source_persona_type_ = std::move(other.source_persona_type_);
  data_type_ = std::move(other.data_type_);
  destination_ = std::move(other.destination_);
  source_ = std::move(other.source_);
  content_ = std::move(other.content_);
  signature_ = std::move(other.signature_);
  return *this;
}

// TODO(Fraser#5#): 2012-12-24 - Once MSVC eventually handles delegating constructors, we can make
//                  this more efficient by using a lambda which returns the parsed protobuf
//                  inside a private constructor taking a single arg of type protobuf.
Message::Message(const serialised_type& serialised_message)
    : action_type_(),
      destination_persona_type_(),
      source_persona_type_(),
      data_type_(),
      destination_(),
      source_(),
      content_(),
      signature_() {
  protobuf::Message proto_message;
  if (!proto_message.ParseFromString(serialised_message.data.string()))
    ThrowError(NfsErrors::message_parsing_error);
  action_type_ = static_cast<ActionType>(proto_message.action_type());
  destination_persona_type_ = static_cast<PersonaType>(proto_message.destination_persona_type());
  source_persona_type_ = static_cast<PersonaType>(proto_message.source_persona_type());
  data_type_ = proto_message.data_type();
  destination_ = NodeId(proto_message.destination());
  source_ = NodeId(proto_message.source());
  if (proto_message.has_content())
    content_ = NonEmptyString(proto_message.content());
  if (proto_message.has_signature())
    signature_ = asymm::Signature(proto_message.signature());
  if (!ValidateInputs())
    ThrowError(NfsErrors::invalid_parameter);
}

bool Message::ValidateInputs() const {
  return (data_type_ >= 0) && !destination_.IsZero() && !source_.IsZero();
}

Message::serialised_type Message::Serialise() const {
  serialised_type serialised_message;
  try {
    protobuf::Message proto_message;
    proto_message.set_action_type(static_cast<int32_t>(action_type_));
    proto_message.set_destination_persona_type(static_cast<int32_t>(destination_persona_type_));
    proto_message.set_source_persona_type(static_cast<int32_t>(source_persona_type_));
    proto_message.set_data_type(data_type_);
    proto_message.set_destination(destination_.string());
    proto_message.set_source(source_.string());
    if (content_.IsInitialised())
      proto_message.set_content(content_.string());
    if (signature_.IsInitialised())
      proto_message.set_signature(signature_.string());

    serialised_message = serialised_type(NonEmptyString(proto_message.SerializeAsString()));
  }
  catch(const std::system_error&) {
    ThrowError(NfsErrors::invalid_parameter);
  }
  return serialised_message;
}

}  // namespace nfs

}  // namespace maidsafe