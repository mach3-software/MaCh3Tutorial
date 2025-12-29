// MaCh3 includes
#include "nudock.hpp"

int main()
{
    NuDock client;
    client.start_client();

    nlohmann::json message;
    message["request"] = "test 123 :)";
    // message["numb"] = 2.12;

    nlohmann::json new_params;
    new_params["osc_pars"]["Theta12"] = 0.307,
    new_params["osc_pars"]["Theta23"] = 0.561;
    new_params["osc_pars"]["Theta13"] = 0.022;
    new_params["osc_pars"]["Deltam2_21"] = 7.53e-5;
    new_params["osc_pars"]["Deltam2_32"] = 2.494e-3;
    // new_params["osc_pars"]["DeltaCP"] = 0.;
    new_params["osc_pars"]["DeltaCP"] = -1.601;
    new_params["sys_pars"]["MaCCRES"] = 0.01;

    client.send_request("/ping", message);
    client.send_request("/get_parameter_names", nlohmann::json::object());
    client.send_request("/log_likelihood", nlohmann::json::object());
    client.send_request("/set_parameters", new_params);
    client.send_request("/log_likelihood", nlohmann::json::object());
    // client.send_request("/set_asimov_point", nlohmann::json::object());
    
    // Make a loop to change DeltaCP from -3.14 to 3.14
    int total_steps = 10;
    for (int i=0; i<total_steps; i++) {
        new_params["osc_pars"]["DeltaCP"] = -3.14 + i*(6.28/total_steps);
        client.send_request("/set_parameters", new_params);
        // client.send_request("")
        client.send_request("/log_likelihood", nlohmann::json::object());
    }
    return 0;
}
