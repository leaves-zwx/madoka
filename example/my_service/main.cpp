#include <iostream>
#include <string>
#include <rest_rpc/client.hpp>
#include <tuple>

struct configure
{
	std::string hostname;
	std::string port;

	META(hostname, port);
};

configure get_config()
{
	std::ifstream in("client.cfg");
	std::stringstream ss;
	ss << in.rdbuf();

	configure cfg = { "127.0.0.1", "9000" }; //if can't find the config file, give the default value.
	DeSerializer dr;
	try
	{
		dr.Parse(ss.str());
		dr.Deserialize(cfg);
	}
	catch (const std::exception& e)
	{
		//timax::SPD_LOG_ERROR(e.what());
		std::cout << e.what() << std::endl;
	}

	return cfg;
}

struct address
{
	std::string service_name;
	std::string host_name;
	int port;

	bool operator == (const address& adr) const
	{
		return adr.service_name == service_name&&adr.host_name == host_name&&adr.port == port;
	}

	MSGPACK_DEFINE(service_name, host_name, port);
};

using my_type = std::tuple<std::string, int>;
namespace client
{
	TIMAX_DEFINE_PROTOCOL(register_service, address(const std::string&, const std::string&, int));
	TIMAX_DEFINE_PROTOCOL(unregister_service, bool(const std::string&, const std::string&, int));
	TIMAX_DEFINE_PROTOCOL(fetch, my_type(const std::string&));
}

int main()
{
	using namespace timax::rpc;
	using timax::rpc::sync_client;
	using codec_type = msgpack_codec;
	using namespace std;

//	timax::log::get().init("rest_rpc_client.lg");
	auto cfg = get_config();

	using my_sync_client = timax::rpc::sync_client<timax::rpc::msgpack_codec>;
	my_sync_client client;
	client.connect(cfg.hostname, cfg.port);

	try
	{
		auto r = client.call(client::register_service, "myservice", "192.168.2.101", 9000);
		std::cout << r.host_name << std::endl;

//		bool b = client.call(client::unregister_service, "myservice", "192.168.2.103", 9000);

		std::tuple<std::string, int> result = client.call(client::fetch, "myservice"); //return ip and port
		std::cout << "host name: " << std::get<0>(result) << ", port: " << std::get<1>(result) << std::endl;
		result = client.call(client::fetch, "myservice"); //return ip and port
		std::cout << "host name: " << std::get<0>(result) << ", port: " << std::get<1>(result) << std::endl;
		result = client.call(client::fetch, "myservice"); //return ip and port
		std::cout << "host name: " << std::get<0>(result) << ", port: " << std::get<1>(result) << std::endl;
		result = client.call(client::fetch, "myservice"); //return ip and port
		std::cout << "host name: " << std::get<0>(result) << ", port: " << std::get<1>(result) << std::endl;
		result = client.call(client::fetch, "myservice"); //return ip and port
		std::cout << "host name: " << std::get<0>(result) << ", port: " << std::get<1>(result) << std::endl;
	}
	catch (const std::exception& ex)
	{
		//timax::SPD_LOG_ERROR(ex.what());
		std::cout << ex.what() << std::endl;
	}

	getchar();
	return 0;
}