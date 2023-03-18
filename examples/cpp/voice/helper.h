#include <memory>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/client_context.h>
#include <grpcpp/completion_queue.h>
#include <grpcpp/impl/status.h>
#include <grpcpp/grpcpp.h>

class Helper {
public:
    template<typename ProtoType, typename ReqT, typename ResT>
    static grpc::Status unaryRpc(std::unique_ptr<typename ProtoType::Stub>* stub, ReqT* req, ResT* res, grpc::Status (*send)(std::unique_ptr<typename ProtoType::Stub>&, ReqT*, ResT*, grpc::ClientContext*)) {
        grpc::ClientContext context;
        return send(*stub, req, res, &context);
    }

    template<typename ProtoType, typename ReqT, typename ResT>
    static bool asyncRpc(std::unique_ptr<typename ProtoType::Stub>& stub, ReqT* req, ResT* res, std::unique_ptr<grpc::ClientAsyncResponseReader<ResT>> (*getResponseReader)(std::unique_ptr<typename ProtoType::Stub>&, ReqT*, grpc::ClientContext*, grpc::CompletionQueue*)) {
        grpc::ClientContext context;
        grpc::CompletionQueue cq;
        std::unique_ptr<grpc::ClientAsyncResponseReader<ResT>> rpc(getResponseReader(stub, req, &context, &cq));
        grpc::Status status;
        rpc->Finish(res, &status, (void*)1);
        void* got_tag;
        bool ok = false;
        cq.Next(&got_tag, &ok);
        if (ok && got_tag == (void*)1) {
            return true;
        } else {
            return false;
        }
    }
};