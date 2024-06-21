// Provider.hpp
#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <chrono>
#include <mutex>

#include <cpr/cprtypes.h>
#include <cpr/session.h>
#include <nlohmann/json_fwd.hpp>

#include "transaction.hpp"
#include "logs.hpp"

using namespace std::literals;

namespace ethyl
{
struct ReadCallData {
    std::string contractAddress;
    std::string data;
};

struct FeeData {
    uint64_t gasPrice;
    uint64_t maxFeePerGas;
    uint64_t maxPriorityFeePerGas;

    FeeData(uint64_t _gasPrice, uint64_t _maxFeePerGas, uint64_t _maxPriorityFeePerGas)
        : gasPrice(_gasPrice), maxFeePerGas(_maxFeePerGas), maxPriorityFeePerGas(_maxPriorityFeePerGas) {}
};

struct Client {
    std::string name;
    cpr::Url url;
};

struct Provider : public std::enable_shared_from_this<Provider> {

    template <typename Ret>
    using optional_callback = std::function<void(std::optional<Ret>)>;

    using json_result_callback = optional_callback<nlohmann::json>;

    /** Add a RPC backend for interacting with the Ethereum network.
     *
     * The provider does not ensure that no duplicates are added to the list.
     *
     * @param name A label for the type of client being added. This information
     * is stored only for the user to identify the client in the list of
     * clients in a given provider.
     *
     * @returns True if the client was added successfully. False if the `url`
     * was not set.
     */
    Provider(std::chrono::milliseconds request_timeout = 3s);
    ~Provider();

    void addClient(std::string name, std::string url);

    bool connectToNetwork();
    void disconnectFromNetwork();

    uint64_t getTransactionCount(std::string_view address, std::string_view blockTag);
    void getTransactionCountAsync(std::string_view address, std::string_view blockTag, optional_callback<uint64_t> user_cb);
    nlohmann::json callReadFunctionJSON(const ReadCallData& callData, std::string_view blockNumber = "latest");
    void callReadFunctionJSONAsync(const ReadCallData& callData, json_result_callback user_cb, std::string_view blockNumber = "latest");
    std::string    callReadFunction(const ReadCallData& callData, std::string_view blockNumber = "latest");
    std::string    callReadFunction(const ReadCallData& callData, uint64_t blockNumberInt);

    uint32_t getNetworkChainId();
    void getNetworkChainIdAsync(optional_callback<uint32_t> user_cb);
    std::string evm_snapshot();
    void evm_snapshot_async(json_result_callback cb);
    bool evm_revert(std::string_view snapshotId);

    uint64_t evm_increaseTime(std::chrono::seconds seconds);

    std::optional<nlohmann::json> getTransactionByHash(std::string_view transactionHash);
    void getTransactionByHashAsync(std::string_view transactionHash, json_result_callback cb);
    std::optional<nlohmann::json> getTransactionReceipt(std::string_view transactionHash);
    void getTransactionReceiptAsync(std::string_view transactionHash, json_result_callback cb);
    std::vector<LogEntry> getLogs(uint64_t fromBlock, uint64_t toBlock, std::string_view address);
    std::vector<LogEntry> getLogs(uint64_t block, std::string_view address);
    void getLogsAsync(uint64_t fromBlock, uint64_t toBlock, std::string_view address, optional_callback<std::vector<LogEntry>> user_cb);
    void getLogsAsync(uint64_t block, std::string_view address, optional_callback<std::vector<LogEntry>> cb);
    std::string getContractStorageRoot(std::string_view address, uint64_t blockNumberInt);
    std::string getContractStorageRoot(std::string_view address, std::string_view blockNumber = "latest");
    void getContractStorageRootAsync(std::string_view address, optional_callback<std::string> user_cb, uint64_t blockNumberInt);
    void getContractStorageRootAsync(std::string_view address, optional_callback<std::string> user_cb, std::string_view blockNumber = "latest");

    std::string sendTransaction(const Transaction& signedTx);
    std::string sendUncheckedTransaction(const Transaction& signedTx);
    void sendUncheckedTransactionAsync(const Transaction& signedTx, optional_callback<std::string> user_cb);

    uint64_t waitForTransaction(std::string_view txHash, std::chrono::milliseconds timeout = 320s);
    bool transactionSuccessful(std::string_view txHash, std::chrono::milliseconds timeout = 320s);
    uint64_t gasUsed(std::string_view txHash, std::chrono::milliseconds timeout = 320s);
    std::string getBalance(std::string_view address);
    void getBalanceAsync(std::string_view address, optional_callback<std::string> user_cb);
    std::string getContractDeployedInLatestBlock();

    uint64_t getLatestHeight();
    void getLatestHeightAsync(optional_callback<uint64_t> user_cb);
    FeeData getFeeData();

    /// List of clients for interacting with the Ethereum network via RPC
    /// The order of the clients dictates the order in which a request is
    /// attempted.
    std::vector<Client>                      clients;

    uint64_t next_request_id{0};
    std::map<uint64_t, std::pair<cpr::AsyncWrapper<std::optional<nlohmann::json>>, json_result_callback>> pending_requests;
    std::unique_ptr<std::thread> response_thread;
    std::condition_variable response_cv;
    std::queue<uint64_t> pending_responses;
    bool running{true};

private:
    /**
     * @param timeout Set a timeout for the provider to connect to current
     * client it's attempting before returning a timeout failure. If this is not
     * set,the timeout is the default timeout value of CURL which is 300
     * seconds.
     *
     * If you have multiple clients it may be desired to set this value such
     * that the provider can quickly evaluate the backup clients in its list on
     * failure.
     */
    void makeJsonRpcRequest(std::string_view method,
                                     const nlohmann::json& params,
                                     json_result_callback cb);
    std::optional<nlohmann::json> makeJsonRpcRequest(std::string_view method,
                                     const nlohmann::json& params);
    std::shared_ptr<cpr::Session> session = std::make_shared<cpr::Session>();
    std::mutex mutex;
};
}; // namespace ethyl
