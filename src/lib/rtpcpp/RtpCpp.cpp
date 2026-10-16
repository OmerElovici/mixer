#include "RtpCpp.hpp"

#include <limits>
#include <random>
#include <stdexcept>



namespace RtpCpp {

bool is_audio_pt(const std::uint8_t payload_type) noexcept {
    using enum AudioPt;
    switch (AudioPt{
        payload_type,
    }) {
    case kPCMU:
    case kGSM:
    case kG723:
    case kDVI4_8000:
    case kDVI4_16000:
    case kLPC:
    case kPCMA:
    case kG722:
    case kL16_2CH:
    case kL16_1CH:
    case kQCELP:
    case kCN:
    case kMPA:
    case kG728:
    case kDVI4_11025:
    case kDVI4_22050:
    case kG729:
        return true;

    default:
        return false;
    }
}

bool is_video_pt(const std::uint8_t payload_type) noexcept {
    using enum VideoPt;

    switch (VideoPt{
        payload_type,
    }) {
    case kCelB:
    case kJPEG:
    case kNV:
    case kH261:
    case kMPV:
    case kH263:
        return true;
    case kMP2T:
        break;
    }

    return false;
}

std::string_view audio_pt_tostring(const std::uint8_t payload_type) noexcept {
    using enum AudioPt;
    switch (AudioPt{
        payload_type,
    }) {
    case kPCMU:
        return "PCMU";
    case kGSM:
        return "GSM";
    case kG723:
        return "G723";
    case kDVI4_8000:
        return "DVI (8000 hz)";
    case kDVI4_16000:
        return "DVI (16000 hz)";
    case kLPC:
        return "LPC";
    case kPCMA:
        return "PCMA";
    case kG722:
        return "G722";
    case kL16_2CH:
        return "L16 (dual channel)";
    case kL16_1CH:
        return "L16 (single channel)";
    case kQCELP:
        return "QCELP";
    case kCN:
        return "CN";
    case kMPA:
        return "MPA";
    case kG728:
        return "G729";
    case kDVI4_11025:
        return "DVI4 (11020 hz)";
    case kDVI4_22050:
        return "DVI (22050 hz)";
    case kG729:
        return "G729";
    }

    return {};
}

std::string_view video_pt_tostring(const std::uint8_t payload_type) noexcept {
    using enum VideoPt;
    switch (VideoPt{
        payload_type,
    }) {
    case kCelB:
        return "CelB";
    case kJPEG:
        return "JPEG";
    case kNV:
        return "NV";
    case kH261:
        return "H261";
    case kMPV:
        return "MPV";
    case kH263:
        return "H263";
    case kMP2T:
        return "MP2T";
        break;
    }

    return {};
}

bool is_dynamic_rtp(const std::uint8_t payload_type) noexcept {
    return payload_type >= kMinDynamicRtp && payload_type <= kMaxDynamicRtp;
}

bool is_valid_pt(const std::uint8_t payload_type) noexcept {
    return is_audio_pt(payload_type) || is_video_pt(payload_type) || is_dynamic_rtp(payload_type);
}

} // namespace RtpCpp




namespace RtpCpp {

const char* RtpPacketCatagory::name() const noexcept { // NOLINT
    return "MyAppErrorCategory";
}

std::string RtpPacketCatagory::message(int ev) const { // NOLINT
    switch (static_cast<Result>(ev)) {
    case Result::kSuccess:
        return "Success";
    case Result::kBufferTooSmall:
        return "Buffer too small";
    case Result::kParseBufferOverflow:
        return "Parse overflow buffer length";
    case Result::kInvalidHeaderLength:
        return "Invalid header length";
    case Result::kInvalidCsrcCount:
        return "Invalid csrc count";
    case Result::kFixedBufferTooSmall:
        return "Fixed buffer too small";
    case Result::kParseExtensionOverflow:
        return "Extension overflow buffer length";
    default:
        return "Unknown internal error.";
    }
}

const std::error_category& get_rtp_packet_catagory() noexcept {
    const static RtpPacketCatagory instance;
    return instance;
}

// 4. ADL helper function (Must be in the SAME namespace as the enum)
std::error_code make_error_code(Result err) noexcept {
    return {static_cast<int>(err), get_rtp_packet_catagory()};
}

} // namespace RtpCpp


// NOLINTBEGIN
/*
 * This is a standalone copy of the SmallVector class from LLVM with no
 * dependencies outside standard library.
 *
 * The original code is part of the LLVM Project, under the Apache License v2.0
 * with LLVM Exceptions. See https://llvm.org/LICENSE.txt for license
 * information. SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 *
 * This standalone copy was made independent of LLVM and is distributed under
 * the same license.
 */
//===----------------------------------------------------------------------===//
//
// This file implements the SmallVector class.
//
//===----------------------------------------------------------------------===//

// #include "llvm/ADT/Twine.h"
// #include "llvm/Support/MemAlloc.h"
#ifdef LLVM_ENABLE_EXCEPTIONS
    #include <stdexcept>
#endif
using namespace RtpCpp::llvm;

static void* safe_malloc(size_t size) {
    void* p = std::malloc(size);
    if (!p) {
#ifdef __cpp_exceptions
        throw std::bad_alloc();
#else
        std::abort();
#endif
    }
    return p;
}

static void* safe_realloc(void* ptr, size_t size) {
    void* p = std::realloc(ptr, size);
    if (!p) {
#ifdef __cpp_exceptions
        throw std::bad_alloc();
#else
        std::abort();
#endif
    }
    return p;
}

// Check that no bytes are wasted and everything is well-aligned.
namespace {
// These structures may cause binary compat warnings on AIX. Suppress the
// warning since we are only using these types for the static assertions below.
#if defined(_AIX)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Waix-compat"
#endif
struct Struct16B {
    alignas(16) void* X;
};

struct Struct32B {
    alignas(32) void* X;
};
#if defined(_AIX)
    #pragma GCC diagnostic pop
#endif
} // namespace

static_assert(
    sizeof(SmallVector<void*, 0>) == sizeof(unsigned) * 2 + sizeof(void*),
    "wasted space in SmallVector size 0");
static_assert(
    alignof(SmallVector<Struct16B, 0>) >= alignof(Struct16B),
    "wrong alignment for 16-byte aligned T");
static_assert(
    alignof(SmallVector<Struct32B, 0>) >= alignof(Struct32B),
    "wrong alignment for 32-byte aligned T");
static_assert(
    sizeof(SmallVector<Struct16B, 0>) >= alignof(Struct16B),
    "missing padding for 16-byte aligned T");
static_assert(
    sizeof(SmallVector<Struct32B, 0>) >= alignof(Struct32B),
    "missing padding for 32-byte aligned T");
static_assert(
    sizeof(SmallVector<void*, 1>) == sizeof(unsigned) * 2 + sizeof(void*) * 2,
    "wasted space in SmallVector size 1");

static_assert(
    sizeof(SmallVector<char, 0>) == sizeof(void*) * 2 + sizeof(void*),
    "1 byte elements have word-sized type for size and capacity");

/// Report that MinSize doesn't fit into this vector's size type. Throws
/// std::length_error or calls report_fatal_error.
[[noreturn]] static void report_size_overflow(size_t MinSize, size_t MaxSize);

static void report_size_overflow(size_t MinSize, size_t MaxSize) {
    std::string Reason =
        "SmallVector unable to grow. Requested capacity (" + std::to_string(MinSize) +
        ") is larger than maximum value for size type (" + std::to_string(MaxSize) + ")";
#ifdef LLVM_ENABLE_EXCEPTIONS
    throw std::length_error(Reason);
#else
    #ifdef __cpp_exceptions
    throw std::length_error(Reason);
    #else
    std::abort();
    #endif
#endif
}

/// Report that this vector is already at maximum capacity. Throws
/// std::length_error or calls report_fatal_error.
[[noreturn]] static void report_at_maximum_capacity(size_t MaxSize);

static void report_at_maximum_capacity(size_t MaxSize) {
    std::string Reason =
        "SmallVector capacity unable to grow. Already at maximum size " + std::to_string(MaxSize);
#ifdef LLVM_ENABLE_EXCEPTIONS
    throw std::length_error(Reason);
#else
    #ifdef __cpp_exceptions
    throw std::length_error(Reason);
    #else
    std::abort();
    #endif
#endif
}

// Note: Moving this function into the header may cause performance regression.
template <class Size_T>
static size_t getNewCapacity(size_t MinSize, [[maybe_unused]] size_t TSize, size_t OldCapacity) {
    constexpr size_t MaxSize = std::numeric_limits<Size_T>::max();

    // Ensure we can fit the new capacity.
    // This is only going to be applicable when the capacity is 32 bit.
    if (MinSize > MaxSize)
        report_size_overflow(MinSize, MaxSize);

    // Ensure we can meet the guarantee of space for at least one more element.
    // The above check alone will not catch the case where grow is called with a
    // default MinSize of 0, but the current capacity cannot be increased.
    // This is only going to be applicable when the capacity is 32 bit.
    if (OldCapacity == MaxSize)
        report_at_maximum_capacity(MaxSize);

    // In theory 2*capacity can overflow if the capacity is 64 bit, but the
    // original capacity would never be large enough for this to be a problem.
    size_t NewCapacity = 2 * OldCapacity + 1; // Always grow.
    return std::clamp(NewCapacity, MinSize, MaxSize);
}

/// If vector was first created with capacity 0, getFirstEl() points to the
/// memory right after, an area unallocated. If a subsequent allocation,
/// that grows the vector, happens to return the same pointer as getFirstEl(),
/// get a new allocation, otherwise isSmall() will falsely return that no
/// allocation was done (true) and the memory will not be freed in the
/// destructor. If a VSize is given (vector size), also copy that many
/// elements to the new allocation - used if realloca fails to increase
/// space, and happens to allocate precisely at BeginX.
/// This is unlikely to be called often, but resolves a memory leak when the
/// situation does occur.
static void* replaceAllocation(void* NewElts, size_t TSize, size_t NewCapacity, size_t VSize = 0) {
    void* NewEltsReplace = safe_malloc(NewCapacity * TSize);
    if (VSize)
        memcpy(NewEltsReplace, NewElts, VSize * TSize);
    free(NewElts);
    return NewEltsReplace;
}

// Note: Moving this function into the header may cause performance regression.
template <class Size_T>
void* SmallVectorBase<Size_T>::mallocForGrow(
    void* FirstEl,
    size_t MinSize,
    size_t TSize,
    size_t& NewCapacity) {
    NewCapacity = getNewCapacity<Size_T>(MinSize, TSize, this->capacity());
    // Even if capacity is not 0 now, if the vector was originally created with
    // capacity 0, it's possible for the malloc to return FirstEl.
    void* NewElts = safe_malloc(NewCapacity * TSize);
    if (NewElts == FirstEl)
        NewElts = replaceAllocation(NewElts, TSize, NewCapacity);
    return NewElts;
}

// Note: Moving this function into the header may cause performance regression.
template <class Size_T>
void SmallVectorBase<Size_T>::grow_pod(void* FirstEl, size_t MinSize, size_t TSize) {
    size_t NewCapacity = getNewCapacity<Size_T>(MinSize, TSize, this->capacity());
    void* NewElts;
    if (BeginX == FirstEl) {
        NewElts = safe_malloc(NewCapacity * TSize);
        if (NewElts == FirstEl)
            NewElts = replaceAllocation(NewElts, TSize, NewCapacity);

        // Copy the elements over.  No need to run dtors on PODs.
        memcpy(NewElts, this->BeginX, size() * TSize);
    } else {
        // If this wasn't grown from the inline copy, grow the allocated space.
        NewElts = safe_realloc(this->BeginX, NewCapacity * TSize);
        if (NewElts == FirstEl)
            NewElts = replaceAllocation(NewElts, TSize, NewCapacity, size());
    }

    this->set_allocation_range(NewElts, NewCapacity);
}

template class RtpCpp::llvm::SmallVectorBase<uint32_t>;

// Disable the uint64_t instantiation for 32-bit builds.
// Both uint32_t and uint64_t instantiations are needed for 64-bit builds.
// This instantiation will never be used in 32-bit builds, and will cause
// warnings when sizeof(Size_T) > sizeof(size_t).
#if SIZE_MAX > UINT32_MAX
template class RtpCpp::llvm::SmallVectorBase<uint64_t>;

// Assertions to ensure this #if stays in sync with SmallVectorSizeType.
static_assert(
    sizeof(SmallVectorSizeType<char>) == sizeof(uint64_t),
    "Expected SmallVectorBase<uint64_t> variant to be in use.");
#else
static_assert(
    sizeof(SmallVectorSizeType<char>) == sizeof(uint32_t),
    "Expected SmallVectorBase<uint32_t> variant to be in use.");
#endif
// NOLINTEND



namespace RtpCpp {

RtpTransport::RtpTransport(asio::any_io_executor executor)
    : executor_(std::move(executor)) {}

std::expected<void, std::error_code> RtpTransport::bind(
    std::string_view address,
    unsigned short port) {
    const auto endpoint = Detail::make_udp_endpoint(address, port);
    if (!endpoint) {
        return std::unexpected(endpoint.error());
    }
    return bind(endpoint.value());
}

std::expected<void, std::error_code> RtpTransport::bind(const asio::ip::udp::endpoint& local) {
    auto socket = std::make_shared<asio::ip::udp::socket>(executor_);
    const auto result = Detail::open_and_bind_udp_socket(socket.get(), local);
    if (!result) {
        return result;
    }
    asio_error_code error;
    const auto endpoint = socket->local_endpoint(error);
    if (error) {
        return std::unexpected(error);
    }
    socket_ = std::move(socket);
    set_local_endpoint(endpoint);
    return {};
}

std::expected<void, std::error_code> RtpTransport::bind(
    std::shared_ptr<asio::ip::udp::socket> socket) {
    if (!socket) {
        return std::unexpected(to_error_code(asio::error::bad_descriptor));
    }
    asio_error_code error;
    const auto endpoint = socket->local_endpoint(error);
    if (error) {
        return std::unexpected(error);
    }
    socket_ = std::move(socket);
    executor_ = socket_->get_executor();
    set_local_endpoint(endpoint);
    return {};
}

const asio::any_io_executor& RtpTransport::get_executor() {
    return executor_;
}

asio::ip::udp::socket* RtpTransport::get_socket() {
    return socket_.get();
}

void RtpTransport::clear_local_endpoint() {
    set_local_endpoint({});
}

std::expected<asio::ip::udp::endpoint, std::error_code> RtpTransport::local_endpoint() const {
    std::scoped_lock<std::mutex> const lock{local_endpoint_mtx_};
    return local_endpoint_;
}

void RtpTransport::set_local_endpoint(const asio::ip::udp::endpoint& endpoint) {
    std::scoped_lock<std::mutex> const lock{local_endpoint_mtx_};
    local_endpoint_ = endpoint;
}

void RtpTransport::apply_socket_action(bool close_socket) {
    if (!socket_) {
        return;
    }
    asio_error_code error;
    if (close_socket) {
        (void)socket_->close(error); // NOLINT(bugprone-unused-return-value)
    } else {
        (void)socket_->cancel(error); // NOLINT(bugprone-unused-return-value)
    }
}

} // namespace RtpCpp


#ifdef RTPCPP_USE_BOOST_ASIO
    #include <boost/asio/buffer.hpp>
#else
    #include <asio/buffer.hpp>
#endif

namespace RtpCpp {

using namespace Detail;

BasicRawRtpSender::BasicRawRtpSender(asio::any_io_executor executor)
    : transport_(std::move(executor)) {}

std::expected<void, std::error_code> BasicRawRtpSender::bind(
    std::string_view address,
    unsigned short port) {
    return transport_.bind(address, port);
}

std::expected<void, std::error_code> BasicRawRtpSender::bind(const asio::ip::udp::endpoint& local) {
    return transport_.bind(local);
}

std::expected<void, std::error_code> BasicRawRtpSender::bind(
    std::shared_ptr<asio::ip::udp::socket> socket) {
    return transport_.bind(std::move(socket));
}

asio::any_io_executor BasicRawRtpSender::get_executor() {
    return transport_.get_executor();
}

std::expected<asio::ip::udp::endpoint, std::error_code> BasicRawRtpSender::local_endpoint() const {
    return transport_.local_endpoint();
}

bool BasicRawRtpSender::close(CloseCallback callback, bool close_socket) {
    if (close_socket) {
        transport_.clear_local_endpoint();
    }
    asio::post(
        transport_.get_executor(),
        [self = shared_from_this(), callback = std::move(callback), close_socket] mutable {
            self->transport_.apply_socket_action(close_socket);
            invoke_callback(std::move(callback));
        });
    return true;
}

std::expected<void, std::error_code> BasicRawRtpSender::set_default_dst(
    std::string_view address,
    unsigned short port) {
    const auto endpoint = Detail::make_udp_endpoint(address, port);

    if (!endpoint) {
        return std::unexpected(endpoint.error());
    }

    return set_default_dst(endpoint.value());
}

std::expected<void, std::error_code> BasicRawRtpSender::set_default_dst(
    const asio::ip::udp::endpoint& dst) {
    default_dst_ = dst;
    return {};
}

std::optional<asio::ip::udp::endpoint> BasicRawRtpSender::get_default_dst() const {
    return default_dst_;
}

void BasicRawRtpSender::clear_default_dst() {
    default_dst_.reset();
}

template <typename ConcreteHandler>
void BasicRawRtpSender::execute_async_send_to(
    std::span<const std::uint8_t> pkt,
    const asio::ip::udp::endpoint& dst,
    ConcreteHandler&& wrapped_handler) {
    auto* const socket = transport_.get_socket();
    if (socket == nullptr || !socket->is_open()) {
        const auto exec = asio::get_associated_executor(wrapped_handler, transport_.get_executor());
        asio::post(
            transport_.get_executor(),
            bind_callback_executor(
                exec,
                [handler = std::forward<ConcreteHandler>(wrapped_handler)] mutable {
                    std::move(handler)(to_error_code(asio::error::bad_descriptor), std::size_t{});
                }));
        return;
    }

    socket->async_send_to(asio::buffer(pkt), dst, std::forward<ConcreteHandler>(wrapped_handler));
}

void BasicRawRtpSender::async_send_pkt(
    std::span<const std::uint8_t> raw_packet,
    const asio::ip::udp::endpoint& dst,
    OnRawRtpSend callback) {
    const auto exec = asio::get_associated_executor(callback, transport_.get_executor());
    auto bound_handler = bind_callback_executor(
        exec,
        [self = shared_from_this(),
         cab = std::move(callback)](const std::error_code& err, std::size_t bytes_sent) mutable {
            if (err == to_error_code(asio::error::operation_aborted)) {
                return;
            }
            invoke_callback(std::move(cab), bytes_sent, err);
        });

    // The bound handler retains self through dispatch and send completion.
    asio::dispatch(
        transport_.get_executor(),
        [this, raw_packet, dst, handler = std::move(bound_handler)] mutable {
            execute_async_send_to(raw_packet, dst, std::move(handler));
        });
}

void BasicRawRtpSender::async_send_pkt(
    std::span<const std::uint8_t> raw_packet,
    OnRawRtpSend callback) {
    if (!default_dst_) {
        post_callback(
            transport_.get_executor(),
            std::move(callback),
            std::size_t{
                0,
            },
            std::make_error_code(std::errc::destination_address_required));
        return;
    }

    async_send_pkt(raw_packet, *default_dst_, std::move(callback));
}

} // namespace RtpCpp


#ifdef RTPCPP_USE_BOOST_ASIO
#else
#endif
#ifdef RTPCPP_USE_BOOST_ASIO
#else
#endif



namespace RtpCpp {

using namespace Detail;

std::shared_ptr<ConcurrentRawRtpSender> ConcurrentRawRtpSender::create(
    const asio::any_io_executor& executor,
    std::size_t max_channel_size) {
    auto sender = std::shared_ptr<ConcurrentRawRtpSender>(
        new ConcurrentRawRtpSender(executor, max_channel_size));
    sender->start_channel_loop(sender);
    return sender;
}

ConcurrentRawRtpSender::ConcurrentRawRtpSender(
    const asio::any_io_executor& executor,
    std::size_t max_channel_size)
    : sender_(std::make_shared<BasicRawRtpSender>(executor))
    , channel_(executor, max_channel_size) {}

asio::any_io_executor ConcurrentRawRtpSender::get_executor() {
    return channel_.get_executor();
}

std::expected<void, std::error_code> ConcurrentRawRtpSender::bind(
    std::string_view address,
    unsigned short port) {
    return sender_->bind(address, port);
}

std::expected<void, std::error_code> ConcurrentRawRtpSender::bind(
    const asio::ip::udp::endpoint& local) {
    return sender_->bind(local);
}

std::expected<void, std::error_code> ConcurrentRawRtpSender::bind(
    std::shared_ptr<asio::ip::udp::socket> socket) {
    return sender_->bind(std::move(socket));
}

std::expected<void, std::error_code> ConcurrentRawRtpSender::set_default_dst(
    std::string_view address,
    unsigned short port) {
    return sender_->set_default_dst(address, port);
}

std::expected<void, std::error_code> ConcurrentRawRtpSender::set_default_dst(
    const asio::ip::udp::endpoint& dst) {
    return sender_->set_default_dst(dst);
}

void ConcurrentRawRtpSender::clear_default_dst() {
    sender_->clear_default_dst();
}

std::optional<asio::ip::udp::endpoint> ConcurrentRawRtpSender::get_default_dst() const {
    return sender_->get_default_dst();
}

std::expected<asio::ip::udp::endpoint, std::error_code> ConcurrentRawRtpSender::local_endpoint()
    const {
    return sender_->local_endpoint();
}

bool ConcurrentRawRtpSender::close(CloseCallback callback, bool close_socket) {
    return sender_->close(
        [self = shared_from_this(), callback = std::move(callback)] mutable {
            self->channel_.reset();
            invoke_callback(std::move(callback));
        },
        close_socket);
}

void ConcurrentRawRtpSender::async_send_pkt(
    std::span<const std::uint8_t> raw_packet,
    const asio::ip::udp::endpoint& dst,
    OnRawRtpSend callback) {
    enqueue_packet(raw_packet, dst, std::move(callback));
}

void ConcurrentRawRtpSender::async_send_pkt(
    std::span<const std::uint8_t> raw_packet,
    OnRawRtpSend callback) {
    auto const dst = get_default_dst();
    if (!dst) {
        post_callback(
            channel_.get_executor(),
            std::move(callback),
            std::size_t{
                0,
            },
            std::make_error_code(std::errc::destination_address_required));
        return;
    }

    enqueue_packet(raw_packet, dst.value(), std::move(callback));
}

void ConcurrentRawRtpSender::enqueue_packet(
    std::span<const std::uint8_t> raw_packet,
    const asio::ip::udp::endpoint& dst,
    OnRawRtpSend callback) {
    ChannelData data{
        .remote_ = dst,
        .buffer_ = std::vector<std::uint8_t>(raw_packet.begin(), raw_packet.end()),
        .callback_ = std::move(callback),
    };
    if (!channel_.try_send(asio_error_code{}, std::move(data))) {
        // TODO we should propogate to user if we fail to push to buffer
        //  post_callback(
        //      channel_.get_executor(),
        //      {},
        //      0UL,
        //      to_error_code(asio::error::no_buffer_space));
    }
}

void ConcurrentRawRtpSender::handle_packet(
    std::vector<std::uint8_t> buffer,
    const asio::ip::udp::endpoint& dst,
    OnRawRtpSend callback) {
    const std::span<const std::uint8_t> pkt_span(buffer);
    const auto exec = asio::get_associated_executor(callback, channel_.get_executor());
    auto bound_handler = bind_callback_executor(
        exec,
        [pkt_buf = std::move(buffer),
         cab = std::move(callback)](std::size_t bytes_sent, const std::error_code& err) mutable {
            if (err == RtpCpp::to_error_code(asio::error::operation_aborted)) {
                return;
            }
            invoke_callback(std::move(cab), bytes_sent, err);
        });

    sender_->async_send_pkt(pkt_span, dst, std::move(bound_handler));
}

void ConcurrentRawRtpSender::start_channel_loop(std::shared_ptr<ConcurrentRawRtpSender> self) {
    for (;;) {
        const bool rec = channel_.try_receive([this](
                                                  const RtpCpp::asio_error_code& err,
                                                  ChannelData data) {
            if (err) {
                if (err != RtpCpp::to_error_code(asio::error::operation_aborted) &&
                    data.callback_) {
                    dispatch_callback(channel_.get_executor(), std::move(data.callback_), 0UL, err);
                }
                return;
            }
            handle_packet(std::move(data.buffer_), data.remote_, std::move(data.callback_));
        });
        if (!rec) {
            break;
        }
    }

    channel_.async_receive([self = std::move(self)](
                               const RtpCpp::asio_error_code& err,
                               ChannelData data) mutable {
        if (err) {
            if (err != RtpCpp::to_error_code(asio::error::operation_aborted) && data.callback_) {
                dispatch_callback(self->channel_.get_executor(), std::move(data.callback_), 0, err);
            }
            // in case loop stopped and user didnt supply callback.
            if (err != RtpCpp::to_error_code(asio::error::operation_aborted) &&
                err != asio::experimental::error::channel_closed) {
                auto* raw_this = self.get();
                raw_this->start_channel_loop(std::move(self));
            }
            return;
        }

        self->handle_packet(std::move(data.buffer_), data.remote_, std::move(data.callback_));
        auto* raw_this = self.get();
        raw_this->start_channel_loop(std::move(self));
    });
}

} // namespace RtpCpp


#ifdef RTPCPP_USE_BOOST_ASIO
#else
#endif


namespace RtpCpp {

using namespace Detail;

static std::error_code process_datagram(
    const asio::ip::udp::endpoint& sender_info,
    RtpPacketView& pkt,
    std::size_t bytes_received,
    const std::optional<asio::ip::udp::endpoint>& source_filter) {
    if (source_filter && sender_info != *source_filter) {
        // TODO this is kinda inaccurate and maybe will conflict with asio errors?
        return std::make_error_code(std::errc::invalid_argument);
    }

    return pkt.parse(bytes_received);
}

RtpReceiver::RtpReceiver(asio::any_io_executor executor)
    : transport_(std::move(executor)) {}

bool RtpReceiver::start_recv_loop(OnRtpPacket callback, const asio::any_io_executor& executor) {
    if (callback == nullptr) {
        return false;
    }

    std::uint8_t expected = std::to_underlying(StateMask::kIdle);
    if (!state_.compare_exchange_strong(
            expected, std::to_underlying(StateMask::kLooping), std::memory_order_relaxed)) {
        return false;
    }


    asio::dispatch(
        transport_.get_executor(),
        [self = shared_from_this(), callback = std::move(callback), executor] mutable {
            self->on_rtp_pkt_ = std::move(callback);
            self->loop_exec_ = (executor != nullptr) ? executor : self->transport_.get_executor();
            self->do_recv_loop_impl();
        });
    return true;
}

void RtpReceiver::do_recv_loop_impl() {
    if (stop_action_ != StopAction::kNone) {
        invoke_close_callback();
        return;
    }

    auto* const socket = transport_.get_socket();
    if (socket == nullptr || !socket->is_open()) {
        asio::post(loop_exec_, [self = shared_from_this()] {
            const RtpPacketView pkt{};
            const asio::ip::udp::endpoint sender{};
            const auto err = to_error_code(asio::error::bad_descriptor);

            (void)self->on_rtp_pkt_(pkt, sender, err);

            self->invoke_close_callback();
        });
        return;
    }


    auto executor_cab = bind_callback_executor(
        loop_exec_,
        [self = shared_from_this()](const std::error_code& err, std::size_t bytes_received) {
            bool continue_loop = true;
            RtpPacketView pkt{std::span<std::uint8_t>{self->rx_buffer_.data(), bytes_received}};

            if (err) {
                if (err == to_error_code(asio::error::operation_aborted) ||
                    err == to_error_code(asio::error::bad_descriptor)) {
                    (void)self->on_rtp_pkt_(pkt, self->sender_info_, err);
                    self->invoke_close_callback();
                    return;
                }

                continue_loop = self->on_rtp_pkt_(pkt, self->sender_info_, err);
                if (continue_loop) {
                    self->do_recv_loop();
                    return;
                }

                self->invoke_close_callback();
                return;
            }

            const std::error_code process_res =
                process_datagram(self->sender_info_, pkt, bytes_received, self->source_filter_);

            continue_loop = self->on_rtp_pkt_(pkt, self->sender_info_, process_res);

            if (continue_loop) {
                self->do_recv_loop();
            } else {
                self->invoke_close_callback();
            }
        });


    socket->async_receive_from(asio::buffer(rx_buffer_), sender_info_, executor_cab);
}

void RtpReceiver::do_recv_loop() {
    asio::dispatch(transport_.get_executor(), [self = shared_from_this()] {
        self->do_recv_loop_impl();
    });
}

std::expected<void, std::error_code> RtpReceiver::bind(
    std::string_view address,
    unsigned short port) {
    const auto result = transport_.bind(address, port);
    if (result) {
        state_.store(std::to_underlying(StateMask::kIdle), std::memory_order_relaxed);
    }
    return result;
}

std::expected<void, std::error_code> RtpReceiver::bind(const asio::ip::udp::endpoint& local) {
    const auto result = transport_.bind(local);
    if (result) {
        state_.store(std::to_underlying(StateMask::kIdle), std::memory_order_relaxed);
    }
    return result;
}

std::expected<void, std::error_code> RtpReceiver::bind(
    std::shared_ptr<asio::ip::udp::socket> socket) {
    const auto result = transport_.bind(std::move(socket));
    if (result) {
        state_.store(std::to_underlying(StateMask::kIdle), std::memory_order_relaxed);
    }
    return result;
}

asio::any_io_executor RtpReceiver::get_executor() {
    return transport_.get_executor();
}

std::expected<asio::ip::udp::endpoint, std::error_code> RtpReceiver::local_endpoint() const {
    return transport_.local_endpoint();
}

bool RtpReceiver::close(CloseCallback callback, bool close_socket) {
    const auto previous =
        state_.fetch_or(std::to_underlying(StateMask::kStopping), std::memory_order_relaxed);
    if ((previous & std::to_underlying(StateMask::kStopping)) != 0) {
        return false;
    }

    if (close_socket) {
        transport_.clear_local_endpoint();
    }

    asio::post(
        transport_.get_executor(),
        [self = shared_from_this(), callback = std::move(callback), close_socket] mutable {
            self->transport_.apply_socket_action(close_socket);
            self->close_cab_ = std::move(callback);
            self->stop_action_ = close_socket ? StopAction::kClose : StopAction::kCancel;
            // Even if cancellation fails, an outstanding receive must finish first.
            if ((self->state_.load(std::memory_order_relaxed) &
                 std::to_underlying(StateMask::kLooping)) == 0) {
                self->finish_close();
            }
        });
    return true;
}

void RtpReceiver::invoke_close_callback() {
    asio::dispatch(transport_.get_executor(), [self = shared_from_this()] {
        if (self->stop_action_ != StopAction::kNone) {
            self->finish_close();
        } else {
            self->on_rtp_pkt_ = nullptr;
            self->state_.fetch_and(
                static_cast<std::uint8_t>(~std::to_underlying(StateMask::kLooping)),
                std::memory_order_relaxed);
        }
    });
}

void RtpReceiver::finish_close() {
    on_rtp_pkt_ = nullptr;
    auto callback = std::exchange(close_cab_, nullptr);
    auto* const socket = transport_.get_socket();
    const bool can_restart =
        stop_action_ == StopAction::kCancel && socket != nullptr && socket->is_open();
    stop_action_ = StopAction::kNone;
    state_.store(
        std::to_underlying(can_restart ? StateMask::kIdle : StateMask::kClosed),
        std::memory_order_relaxed);
    if (callback) {
        callback();
    }
}

void RtpReceiver::async_receive_pkt(OnRtpRecvFrom callback) {
    asio::dispatch(
        transport_.get_executor(),
        [self = shared_from_this(), cab = std::move(callback)] mutable {
            self->async_receive_pkt_impl(std::move(cab));
        });
}

void RtpReceiver::set_source_filter(std::optional<asio::ip::udp::endpoint> source_filter) {
    source_filter_ = std::move(source_filter);
}

void RtpReceiver::async_receive_pkt_impl(OnRtpRecvFrom callback) {
    auto* const socket = transport_.get_socket();
    if (socket == nullptr || !socket->is_open()) {
        post_callback(
            transport_.get_executor(),
            std::move(callback),
            RtpPacketView{std::span<std::uint8_t>{}},
            asio::ip::udp::endpoint{},
            to_error_code(asio::error::bad_descriptor));
        return;
    }

    const auto buff = asio::buffer(rx_buffer_);
    const auto self = shared_from_this();

    const auto exec = asio::get_associated_executor(callback, transport_.get_executor());
    socket->async_receive_from(
        buff,
        sender_info_,
        bind_callback_executor(
            exec,
            [self, cab = std::move(callback)](
                const std::error_code& err,
                std::size_t bytes_received) mutable {
                RtpPacketView pkt{std::span<std::uint8_t>{self->rx_buffer_.data(), bytes_received}};
                if (err) {
                    invoke_callback(std::move(cab), pkt, self->sender_info_, err);
                    return;
                }


                const std::error_code process_res =
                    process_datagram(self->sender_info_, pkt, bytes_received, self->source_filter_);
                if (process_res == std::errc::invalid_argument) {
                    self->async_receive_pkt(std::move(cab));
                    return;
                }

                invoke_callback(std::move(cab), pkt, self->sender_info_, process_res);
            }));
}


} // namespace RtpCpp



namespace RtpCpp {

RtpSenderState::RtpSenderState(std::uint32_t ssrc)
    : timestamp_(Utils::generate_timestamp_offset()) {
    if (ssrc == 0) {
        ssrc_ = RtpCpp::Utils::generate_ssrc();
    } else {
        ssrc_ = ssrc;
    }
}

RtpSenderState::UpdatedState RtpSenderState::update_state() {
    const auto timestamp = timestamp_;
    timestamp_ += timestamp_increment_;
    const auto seq = sequence_number_;
    ++sequence_number_;

    return {
        .ssrc_ = ssrc_,
        .timestamp_ = timestamp,
        .sequence_number_ = seq,
        .payload_type_ = payload_type_,
        .marked_ = false,
    };
}

RtpSenderState::UpdatedState RtpSenderState::update_state(std::uint32_t timestamp, bool mark) {
    timestamp_ = timestamp + timestamp_increment_;
    const auto seq = sequence_number_;
    ++sequence_number_;

    return {
        .ssrc_ = ssrc_,
        .timestamp_ = timestamp,
        .sequence_number_ = seq,
        .payload_type_ = payload_type_,
        .marked_ = mark,
    };
}

void RtpSenderState::set_payload_type(std::uint8_t payload_type) {
    payload_type_ = payload_type;
}

void RtpSenderState::set_timestamp_increment(std::uint32_t ts_increment) {
    timestamp_increment_ = ts_increment;
}

} // namespace RtpCpp


#ifdef RTPCPP_USE_BOOST_ASIO
#else
#endif

namespace RtpCpp {

using namespace Detail;

static void write_state_to_pkt(RtpPacketView& pkt, const RtpSenderState::UpdatedState& state) {
    pkt.set_timestamp(state.timestamp_);
    pkt.set_payload_type(state.payload_type_);
    pkt.set_ssrc(state.ssrc_);
    pkt.set_sequence_number(state.sequence_number_);
    pkt.set_marker(state.marked_);
}

RtpSender::SendContext::SendContext(
    std::shared_ptr<IRawRtpSender> raw_sender,
    RtpSenderConfig config)
    : state_(config.ssrc_)
    , raw_sender_(std::move(raw_sender)) {
    state_.set_payload_type(config.payload_type_);
    state_.set_timestamp_increment(config.default_timestamp_step_);
}

RtpSender::RtpSender(std::shared_ptr<IRawRtpSender> raw_sender, RtpSenderConfig config)
    : context_(std::make_shared<SendContext>(std::move(raw_sender), config)) {}

asio::any_io_executor RtpSender::get_executor() {
    return context_->raw_sender_->get_executor();
}

std::expected<void, std::error_code> RtpSender::bind(
    std::string_view address,
    unsigned short port) {
    return context_->raw_sender_->bind(address, port);
}

std::expected<void, std::error_code> RtpSender::bind(const asio::ip::udp::endpoint& local) {
    return context_->raw_sender_->bind(local);
}

std::expected<void, std::error_code> RtpSender::bind(
    std::shared_ptr<asio::ip::udp::socket> socket) {
    return context_->raw_sender_->bind(std::move(socket));
}

std::expected<asio::ip::udp::endpoint, std::error_code> RtpSender::local_endpoint() const {
    return context_->raw_sender_->local_endpoint();
}

std::expected<void, std::error_code> RtpSender::set_default_dst(
    std::string_view address,
    unsigned short port) {
    return context_->raw_sender_->set_default_dst(address, port);
}

std::expected<void, std::error_code> RtpSender::set_default_dst(
    const asio::ip::udp::endpoint& dst) {
    return context_->raw_sender_->set_default_dst(dst);
}

std::optional<asio::ip::udp::endpoint> RtpSender::get_default_dst() const {
    return context_->raw_sender_->get_default_dst();
}

void RtpSender::clear_default_dst() {
    context_->raw_sender_->clear_default_dst();
}

bool RtpSender::close(CloseCallback callback, bool close_socket) {
    return context_->raw_sender_->close(std::move(callback), close_socket);
}

void RtpSender::async_send_pkt(
    std::span<const std::uint8_t> payload,
    std::uint32_t timestamp,
    const asio::ip::udp::endpoint& dst,
    bool marker,
    OnManagedRtpSend callback) {
    async_send_pkt_impl(payload, timestamp, dst, marker, std::move(callback));
}

void RtpSender::async_send_pkt(
    std::span<const std::uint8_t> payload,
    std::uint32_t timestamp,
    bool marker,
    OnManagedRtpSend callback) {
    async_send_pkt_impl(payload, timestamp, std::nullopt, marker, std::move(callback));
}

void RtpSender::async_send_pkt(
    std::span<const std::uint8_t> payload,
    const asio::ip::udp::endpoint& dst,
    OnManagedRtpSend callback) {
    async_send_pkt_impl(payload, std::nullopt, dst, false, std::move(callback));
}

void RtpSender::async_send_pkt(std::span<const std::uint8_t> payload, OnManagedRtpSend callback) {
    async_send_pkt_impl(payload, std::nullopt, std::nullopt, false, std::move(callback));
}

void RtpSender::async_send_pkt_impl(
    std::span<const std::uint8_t> payload,
    std::optional<std::uint32_t> timestamp,
    std::optional<asio::ip::udp::endpoint> dst,
    bool marker,
    OnManagedRtpSend callback) {
    const auto raw_exec = context_->raw_sender_->get_executor();
    std::vector<std::uint8_t> buffer(payload.size() + kFixedRtpHeaderSize);
    std::ranges::copy(payload, std::span(buffer).subspan(kFixedRtpHeaderSize).begin());

    asio::dispatch(
        raw_exec,
        [context = context_,
         buffer = std::move(buffer),
         timestamp,
         dst,
         marker,
         callback = std::move(callback)] mutable {
            const auto& raw_sender = context->raw_sender_;
            RtpPacketView pkt(buffer);
            const auto current_state = timestamp ? context->state_.update_state(*timestamp, marker)
                                                 : context->state_.update_state();

            write_state_to_pkt(pkt, current_state);

            const auto payload_size =
                narrow_cast<std::uint16_t>(buffer.size() - kFixedRtpHeaderSize);
            const std::error_code err = pkt.set_payload_size(payload_size);
            if (err) {
                post_callback(
                    raw_sender->get_executor(),
                    std::move(callback),
                    std::size_t{},
                    err,
                    std::uint16_t{});
                return;
            }

            const auto exec = asio::get_associated_executor(callback, raw_sender->get_executor());
            const std::span<const std::uint8_t> pkt_span(buffer);
            auto raw_cb = bind_callback_executor(
                exec,
                [buf = std::move(buffer),
                 cab = std::move(callback),
                 seq = current_state.sequence_number_](
                    std::size_t bytes_sent,
                    const std::error_code& error) mutable {
                    invoke_callback(std::move(cab), bytes_sent, error, seq);
                });

            if (dst) {
                raw_sender->async_send_pkt(pkt_span, *dst, std::move(raw_cb));
            } else {
                raw_sender->async_send_pkt(pkt_span, std::move(raw_cb));
            }
        });
}

std::unique_ptr<RtpSender> make_rtp_sender(
    const asio::any_io_executor& executor,
    RtpSenderConfig config) {
    return std::make_unique<RtpSender>(std::make_shared<BasicRawRtpSender>(executor), config);
}

std::unique_ptr<RtpSender> make_concurrent_rtp_sender(
    const asio::any_io_executor& executor,
    RtpSenderConfig config,
    std::size_t queue_capacity) {
    return std::make_unique<RtpSender>(
        ConcurrentRawRtpSender::create(executor, queue_capacity),
        config);
}

} // namespace RtpCpp



namespace RtpCpp {

using namespace Detail;

template <typename RtpSenderType>
RtpSession<RtpSenderType>::RtpSession(
    std::shared_ptr<RtpSenderType> sender,
    std::shared_ptr<RtpReceiver> receiver)
    : sender_(std::move(sender))
    , receiver_(std::move(receiver)) {}

template <typename RtpSenderType>
std::expected<void, std::error_code> RtpSession<RtpSenderType>::bind(
    std::string_view address,
    unsigned short port) {
    const auto endpoint_res = make_udp_endpoint(address, port);


    if (!endpoint_res) {
        return std::unexpected(endpoint_res.error());
    }

    return bind(endpoint_res.value());
}

template <typename RtpSenderType>
std::expected<void, std::error_code> RtpSession<RtpSenderType>::bind(
    const asio::ip::udp::endpoint& local) {
    const auto shared_socket = std::make_shared<asio::ip::udp::socket>(sender_->get_executor());
    const auto skt_res = open_and_bind_udp_socket(shared_socket.get(), local);
    if (!skt_res) {
        return std::unexpected(skt_res.error());
    }

    if (auto res = sender_->bind(shared_socket); !res) {
        return res;
    }

    if (auto res = receiver_->bind(shared_socket); !res) {
        return res;
    }

    return {};
}

template <typename RtpSenderType>
bool RtpSession<RtpSenderType>::close(CloseCallback callback, bool close_socket) {
    return receiver_->close(
        [sender = sender_,
         receiver = receiver_,
         callback = std::move(callback),
         close_socket] mutable {
            (void)sender->close(
                [sender, receiver = std::move(receiver), callback = std::move(callback)] mutable {
                    invoke_callback(std::move(callback));
                },
                close_socket);
        },
        close_socket);
}

template <typename RtpSenderType>
bool RtpSession<RtpSenderType>::stop(CloseCallback callback) {
    return close(std::move(callback), false);
}

template <typename RtpSenderType>
RtpSenderType& RtpSession<RtpSenderType>::sender() const {
    assert(sender_ != nullptr);
    return *sender_;
}

template <typename RtpSenderType>
RtpReceiver& RtpSession<RtpSenderType>::receiver() const {
    assert(receiver_ != nullptr);
    return *receiver_;
}


template class RtpSession<BasicRawRtpSender>;
template class RtpSession<RtpSender>;

RtpSession<BasicRawRtpSender> make_raw_rtp_session(const asio::any_io_executor& executor) {
    return {std::make_shared<BasicRawRtpSender>(executor), std::make_shared<RtpReceiver>(executor)};
}

RtpSession<RtpSender> make_rtp_session(
    const asio::any_io_executor& executor,
    RtpSenderConfig sender_config) {
    return {make_rtp_sender(executor, sender_config), std::make_shared<RtpReceiver>(executor)};
}

RtpSession<RtpSender> make_concurrent_rtp_session(
    const asio::any_io_executor& executor,
    RtpSenderConfig sender_config,
    std::size_t queue_capacity) {
    return {
        make_concurrent_rtp_sender(executor, sender_config, queue_capacity),
        std::make_shared<RtpReceiver>(executor)};
}


} // namespace RtpCpp



namespace RtpCpp {

namespace Detail {

template <typename T>
static T generate_random_id() {
    constexpr T kMin = std::numeric_limits<T>::min();
    constexpr T kMax = std::numeric_limits<T>::max();

    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<T> distr(kMin, kMax);

    return distr(generator);
}

} // namespace Detail

namespace Utils {

std::uint32_t generate_ssrc() {
    return Detail::generate_random_id<std::uint32_t>();
}

std::uint16_t generate_sequence_number() {
    return Detail::generate_random_id<std::uint16_t>();
}

std::uint32_t generate_timestamp_offset() {
    return Detail::generate_random_id<std::uint32_t>();
}

} // namespace Utils

} // namespace RtpCpp



namespace RtpCpp {

MediaClock::MediaClock(std::uint32_t sample_rate_hz)
    : sample_rate_hz_(sample_rate_hz)
    , random_start_offset_(Utils::generate_timestamp_offset())
    , current_timestamp_(random_start_offset_) {}

std::uint32_t MediaClock::to_rtp_timestamp(std::chrono::nanoseconds hardware_time) const noexcept {
    // Calculate the number of ticks passed, truncating securely to 32-bits (which allows standard
    // RTP wrapping)
    const auto ticks = (hardware_time.count() * sample_rate_hz_) / 1'000'000'000;
    return random_start_offset_ + static_cast<std::uint32_t>(ticks);
}

} // namespace RtpCpp


#ifdef RTPCPP_USE_BOOST_ASIO
    #include <boost/asio/ip/address.hpp>
#else
    #include <asio/ip/address.hpp>
#endif


namespace RtpCpp::Detail {

std::expected<asio::ip::udp::endpoint, std::error_code> make_udp_endpoint(
    std::string_view address,
    unsigned short port) {
    RtpCpp::asio_error_code err;
    const auto addr = asio::ip::make_address(std::string(address), err);
    if (err) {
        return std::unexpected(err);
    }
    return asio::ip::udp::endpoint{addr, port};
}

std::expected<void, std::error_code> open_and_bind_udp_socket(
    asio::ip::udp::socket* skt,
    const asio::ip::udp::endpoint& endpoint) {
    RtpCpp::asio_error_code err;

    if (skt == nullptr) {
        return std::unexpected(RtpCpp::to_error_code(asio::error::bad_descriptor));
    }

    (void)skt->open(endpoint.protocol(), err); // NOLINT

    if (err) {
        return std::unexpected(err);
    }

    (void)skt->bind(endpoint, err); // NOLINT

    if (err) {
        return std::unexpected(err);
    }

    return {};
}

} // namespace RtpCpp::Detail


// clang-format off
// NOLINTBEGIN -- Imported decoder; preserve the original implementation and layout.



namespace RtpCpp {


// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
// Adapted for RtpCpp.

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static constexpr uint8_t utf8d[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 00..1f
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 20..3f
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 40..5f
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // 60..7f
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9,   9, // 80..9f
    7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,
    7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7,   7, // a0..bf
    8,   8,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,
    2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2,   2, // c0..df
    0xa, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3, // e0..ef
    0xb, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, // f0..ff
    0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1, // s0..s0
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    1,   0,   1,   1,   1,   1,   1,   0,   1,   0,   1,   1,   1,   1,   1,   1, // s1..s2
    1,   2,   1,   1,   1,   1,   1,   2,   1,   2,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   2,   1,   1,   1,   1,   1,   1,   1,   1, // s3..s4
    1,   2,   1,   1,   1,   1,   1,   1,   1,   2,   1,   1,   1,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   3,   1,   3,   1,   1,   1,   1,   1,   1, // s5..s6
    1,   3,   1,   1,   1,   1,   1,   3,   1,   3,   1,   1,   1,   1,   1,   1,
    1,   3,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1, // s7..s8
};

uint32_t decode(uint32_t* state, uint32_t* codep, uint32_t byte) {
    uint32_t type = utf8d[byte];

    *codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);

    *state = utf8d[256 + *state * 16 + type];
    return *state;
}

[[nodiscard]] std::optional<std::u8string> load_utf8_to_str(std::span<const std::uint8_t> buffer) {
    uint32_t state = UTF8_ACCEPT;
    uint32_t codepoint = 0;

    for (const std::uint8_t idx : buffer) {
        if (decode(&state, &codepoint, idx) == UTF8_REJECT) {
            return std::nullopt;
        }
    }

    // Incompleate utf-8
    if (state != UTF8_ACCEPT) {
        return std::nullopt;
    }

    return std::u8string{buffer.begin(), buffer.end()};
}


} // namespace RtpCpp

// NOLINTEND
// clang-format on

