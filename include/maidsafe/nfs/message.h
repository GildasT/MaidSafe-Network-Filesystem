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

#ifndef MAIDSAFE_NFS_MESSAGE_H_
#define MAIDSAFE_NFS_MESSAGE_H_

#include <string>

#include "maidsafe/common/node_id.h"
#include "maidsafe/common/rsa.h"
#include "maidsafe/common/types.h"

#include "maidsafe/detail/data_type_values.h"

#include "maidsafe/nfs/data_message.h"
#include "maidsafe/nfs/generic_message.h"
#include "maidsafe/nfs/types.h"


namespace maidsafe {

namespace nfs {

namespace protobuf { class Message; }

class Message {
 public:
  typedef TaggedValue<NonEmptyString, struct SerialisedMessageTag> serialised_type;

  Message(int32_t inner_message_type,
          const NonEmptyString& serialised_inner_message,
          const asymm::Signature& signature);
  explicit Message(const serialised_type& serialised_message);
  Message(const Message& other);
  Message& operator=(const Message& other);
  Message(Message&& other);
  Message& operator=(Message&& other);

  serialised_type Serialise() const;

  template<typename InnerMessageType>
  typename InnerMessageType::serialised_type serialised_inner_message() const;
  asymm::Signature signature() const { return signature_; }

 private:
  int32_t inner_message_type_;
  NonEmptyString serialised_inner_message_;
  asymm::Signature signature_;
};

template<typename InnerMessageType>
typename InnerMessageType::serialised_type Message::serialised_inner_message() const {
  return typename InnerMessageType::serialised_type(serialised_inner_message_);
}

}  // namespace nfs

}  // namespace maidsafe

#endif  // MAIDSAFE_NFS_MESSAGE_H_
