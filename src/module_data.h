#ifndef __MODULE_DATA_H_INCLUDED
#define __MODULE_DATA_H_INCLUDED
#include <module_tech_config.h>
#include <logger.hpp>
#include <emulator.h>

/**
 * @brief The module_data class represents a module's data and provides methods to manipulate and retrieve information about the module.
 */
class module_data {
public:
    /**
     * @brief Default constructor for the module_data class.
     */
    module_data() : module_initialized(false), module_opened(false), module_started(false), module_parametrized(false), submodID(0), mid(0), channel(0), mode(0), services_performed(0){
        for (int i = 0; i < MAX_NUMBER_OF_CHANNEL; ++i) {
            WB_Enabled[i] = false;
            SC_Enabled[i] = false;
        }
    }


    /**
     * @brief Prints a report of the module data.
     */
    void report_module_data() const {
        LOG_SAMPLE_MODULE(MODULE_LOG, "Module Data Report:\n");
        LOG_SAMPLE_MODULE(MODULE_LOG, "---------------------\n");
        LOG_SAMPLE_MODULE(MODULE_LOG, "Initialized: %s\n", module_initialized ? "true" : "false");
        LOG_SAMPLE_MODULE(MODULE_LOG, "Opened: %s\n", module_opened ? "true" : "false");
        LOG_SAMPLE_MODULE(MODULE_LOG, "Started: %s\n", module_started ? "true" : "false");
        LOG_SAMPLE_MODULE(MODULE_LOG, "Parametrized: %s\n", module_parametrized ? "true" : "false");
        LOG_SAMPLE_MODULE(MODULE_LOG, "Module ID: %d\n", mid);
        LOG_SAMPLE_MODULE(MODULE_LOG, "Submodule ID: %u\n", submodID);
        LOG_SAMPLE_MODULE(MODULE_LOG, "Channel: %d\n", static_cast<int>(channel));
        LOG_SAMPLE_MODULE(MODULE_LOG, "Mode: %d\n", static_cast<int>(mode));
        LOG_SAMPLE_MODULE(MODULE_LOG, "Services Performed: %d\n", services_performed);
        LOG_SAMPLE_MODULE(MODULE_LOG, "---------------------\n");
    }


    ~module_data() {
        report_module_data();
    }

    /**
     * @brief Initializes the module.
     */
    void module_init(void) { module_initialized = true; }

    /**
     * @brief Opens the module.
     */
    void module_open(void) { module_opened = true; }

    /**
     * @brief Starts the module.
     */
    void module_start(void) { module_started = true; }

    /**
     * @brief Sets the default module ID.
     */
    void set_default_module_ID(void) { mid = TECHNOLOGY_MODULE_ID_SAMPLE_MODULE; }

    /**
     * @brief Sets the submodule ID.
     */
    void set_submodule_ID(uint32_t subID) { submodID = subID; }

    /**
     * @brief Configures the module with the given channel and mode.
     * @param new_channel The new channel value.
     * @param new_mode The new mode value.
     */
    void configure_module(unsigned char new_channel, unsigned char new_mode) {
        channel = new_channel;
        mode = new_mode;
    }
    /**
     * @brief Modify module's variables according to the algorithms of the module.
     */
    void perform_services() { 
        //hidden logic 
        services_performed++;
    }

    /**
     * @brief This functions parametrize the module according to the data provided
     * in the parametrization structure. 
     */
    bool parametrize_module(uint16_t rec_num, uint16_t len, uint8_t* structure_ptr) {
        if (!PARAMETRIZATION_OK) {
            return module_parametrized;
        }

        for (int i = 0; i < MAX_NUMBER_OF_CHANNEL; ++i) {
            int offset = 2 + i * 6;
            if (offset < len) {
                uint8_t channel_byte = structure_ptr[offset];

                WB_Enabled[i] = (channel_byte == 0x10 || channel_byte == 0x14);
                SC_Enabled[i] = (channel_byte == 0x04 || channel_byte == 0x14);
            }
            else {
                WB_Enabled[i] = false;
                SC_Enabled[i] = false;
            }
        }

        module_parametrized = true;
        return module_parametrized;
    }

    /* getters */

    /**
     * @brief Checks if the module is initialized.
     * @return True if the module is initialized, false otherwise.
     */
    bool is_module_initialized() const { return module_initialized; }

    /**
     * @brief Checks if the module is opened.
     * @return True if the module is opened, false otherwise.
     */
    bool is_module_opened() const { return module_opened; }

    /**
     * @brief Checks if the module is started.
     * @return True if the module is started, false otherwise.
     */
    bool is_module_started() const { return module_started; }

    /**
     * @brief Gets the module ID.
     * @return The module ID.
     */
    int get_module_ID() const { return mid; }

    /**
     * @brief Gets the channel value.
     * @return The channel value.
     */
    unsigned char get_channel() const { return channel; }

    /**
     * @brief Gets the mode value.
     * @return The mode value.
     */
    unsigned char get_mode() const { return mode; }

     /**
     * @brief Gets the submodule ID.
     * @return The submodule ID.
     */
    uint32_t get_sub_mod_id() const { return submodID; }

    /**
     * @brief Gets the status of WB_Enabled for a specific channel.
     * @param channel The channel index.
     * @return True if WB diagnostics are enabled for the specified channel, false otherwise.
     */
    bool get_WB_Enabled(int channel) const {
        if (channel >= 0 && channel < MAX_NUMBER_OF_CHANNEL) {
            return WB_Enabled[channel];
        }
        return false;
    }

    /**
     * @brief Gets the status of SC_Enabled for a specific channel.
     * @param channel The channel index.
     * @return True if SC diagnostics are enabled for the specified channel, false otherwise.
     */
    bool get_SC_Enabled(int channel) const {
        if (channel >= 0 && channel < MAX_NUMBER_OF_CHANNEL) {
            return SC_Enabled[channel];
        }
        return false;
    }
    

private:
    bool module_initialized;
    bool module_opened;
    bool module_started;
    bool module_parametrized;
    int mid;
    uint32_t submodID;
    unsigned char channel;
    unsigned char mode;
    int services_performed;
    bool WB_Enabled[MAX_NUMBER_OF_CHANNEL];
    bool SC_Enabled[MAX_NUMBER_OF_CHANNEL];
};

extern module_data tested_module;
#endif