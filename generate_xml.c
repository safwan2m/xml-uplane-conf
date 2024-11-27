#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct array_carrier {
    char name[50];
    uint32_t arfcn_center;
    uint32_t center_channel_bw;
    uint32_t channel_bw;
    char ru_carrier[10];
    char rw_duplex_scheme[10];
    char rw_type[10];
    uint8_t dl_radio_frame_offset;
    uint8_t dl_sfn_offset;
    float gain_correction;
    float gain;
    uint32_t n_ta_offset;
} array_carrier_t;

typedef struct delay_management {
	uint32_t T2a_min_up;
	uint32_t T2a_max_up;
	uint32_t T2a_min_cp_dl;
	uint32_t T2a_max_cp_dl;
	uint32_t Tcp_adv_dl;
	uint32_t Ta3_min;
	uint32_t Ta3_max;
	uint32_t T2a_min_cp_ul;
	uint32_t T2a_max_cp_ul;
} delay_management_t;

typedef struct oai_oru_data {
    array_carrier_t tx_array_carrier;
    array_carrier_t rx_array_carrier;
    delay_management_t delay_management;
} oai_oru_data_t;

void add_element(xmlNodePtr parent, const char *name, const char *content) {
    xmlNewChild(parent, NULL, BAD_CAST name, BAD_CAST content);
}

char *generate_uplane_conf_xml(oai_oru_data_t *oru) {
    // Create a new XML document
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root_node = xmlNewNode(NULL, BAD_CAST "user-plane-configuration");
    xmlNewProp(root_node, BAD_CAST "xmlns", BAD_CAST "urn:o-ran:uplane-conf:1.0");
    xmlDocSetRootElement(doc, root_node);

    // Create tx-array-carriers node
    xmlNodePtr tx_node = xmlNewChild(root_node, NULL, BAD_CAST "tx-array-carriers", NULL);
    add_element(tx_node, "name", oru->tx_array_carrier.name);

    char buffer[50];
    sprintf(buffer, "%d", oru->tx_array_carrier.arfcn_center);
    add_element(tx_node, "absolute-frequency-center", buffer);

    sprintf(buffer, "%u", oru->tx_array_carrier.center_channel_bw);
    add_element(tx_node, "center-of-channel-bandwidth", buffer);

    sprintf(buffer, "%d", oru->tx_array_carrier.channel_bw);
    add_element(tx_node, "channel-bandwidth", buffer);

    add_element(tx_node, "active", oru->tx_array_carrier.ru_carrier);
    add_element(tx_node, "rw-duplex-scheme", oru->tx_array_carrier.rw_duplex_scheme);
    add_element(tx_node, "rw-type", oru->tx_array_carrier.rw_type);

    sprintf(buffer, "%.1f", oru->tx_array_carrier.gain);
    add_element(tx_node, "gain", buffer);

    sprintf(buffer, "%d", oru->tx_array_carrier.dl_radio_frame_offset);
    add_element(tx_node, "downlink-radio-frame-offset", buffer);

    sprintf(buffer, "%d", oru->tx_array_carrier.dl_sfn_offset);
    add_element(tx_node, "downlink-sfn-offset", buffer);

    // Create rx-array-carriers node
    xmlNodePtr rx_node = xmlNewChild(root_node, NULL, BAD_CAST "rx-array-carriers", NULL);
     add_element(rx_node, "name", oru->rx_array_carrier.name);

    sprintf(buffer, "%d", oru->rx_array_carrier.arfcn_center);
    add_element(rx_node, "absolute-frequency-center", buffer);

    sprintf(buffer, "%u", oru->rx_array_carrier.center_channel_bw);
    add_element(rx_node, "center-of-channel-bandwidth", buffer);

    sprintf(buffer, "%d", oru->rx_array_carrier.channel_bw);
    add_element(rx_node, "channel-bandwidth", buffer);

    add_element(rx_node, "active", oru->rx_array_carrier.ru_carrier);

    sprintf(buffer, "%d", oru->rx_array_carrier.dl_radio_frame_offset);
    add_element(rx_node, "downlink-radio-frame-offset", buffer);

    sprintf(buffer, "%d", oru->rx_array_carrier.dl_sfn_offset);
    add_element(rx_node, "downlink-sfn-offset", buffer);

    sprintf(buffer, "%.1f", oru->rx_array_carrier.gain_correction);
    add_element(rx_node, "gain-correction", buffer);

    sprintf(buffer, "%d", oru->rx_array_carrier.n_ta_offset);
    add_element(rx_node, "n-ta-offset", buffer);

    // Save the XML document to a string
    xmlChar *xml_buffer = NULL;
    int buffer_size = 0;
    xmlDocDumpFormatMemoryEnc(doc, &xml_buffer, &buffer_size, "UTF-8", 1);

    // Convert xmlChar* to a regular C string
    char *xml_string = strdup((const char *)xml_buffer);

    // Free the document and the temporary XML buffer
    xmlFree(xml_buffer);
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return xml_string;
}

static void store_tx_array_carriers(xmlNode *node, array_carrier_t *tx_array_carrier)
{
  for (xmlNode *cur_child = node; cur_child; cur_child = cur_child->next) {
    if(cur_child->type == XML_ELEMENT_NODE){
    //   int value = atoi((const char *)xmlNodeGetContent(cur_child));
    int value;

      if (strcmp((const char *)cur_child->name, "name") == 0) {
        strcpy(tx_array_carrier->name, (const char *)xmlNodeGetContent(cur_child));
      } else if (strcmp((const char *)cur_child->name, "absolute-frequency-center") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->arfcn_center = value;
      } else if (strcmp((const char *)cur_child->name, "center-of-channel-bandwidth") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->center_channel_bw = (uint32_t)value;
      } else if (strcmp((const char *)cur_child->name, "channel-bandwidth") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->channel_bw = value;
      } else if (strcmp((const char *)cur_child->name, "active") == 0) {
        strcpy(tx_array_carrier->ru_carrier, (const char *)xmlNodeGetContent(cur_child));
      } else if (strcmp((const char *)cur_child->name, "duplex-scheme") == 0) {
        strcpy(tx_array_carrier->rw_duplex_scheme, (const char *)xmlNodeGetContent(cur_child));
      } else if (strcmp((const char *)cur_child->name, "gain") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->gain = value;
      } else if (strcmp((const char *)cur_child->name, "downlink-radio-frame-offset") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->dl_radio_frame_offset = value;
      } else if (strcmp((const char *)cur_child->name, "downlink-sfn-offset") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        tx_array_carrier->dl_sfn_offset = value;
      }
    }
  }
}

static void store_rx_array_carriers(xmlNode *node, array_carrier_t *rx_array_carrier)
{
  for (xmlNode *cur_child = node; cur_child; cur_child = cur_child->next) {
    if(cur_child->type == XML_ELEMENT_NODE){
    //   int value = atoi((const char *)xmlNodeGetContent(cur_child));
    int value;

      if (strcmp((const char *)cur_child->name, "name") == 0) {
        strcpy(rx_array_carrier->name, (const char *)xmlNodeGetContent(cur_child));
      } else if (strcmp((const char *)cur_child->name, "absolute-frequency-center") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->arfcn_center = value;
      } else if (strcmp((const char *)cur_child->name, "center-of-channel-bandwidth") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->center_channel_bw = (uint32_t)value;
      } else if (strcmp((const char *)cur_child->name, "channel-bandwidth") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->channel_bw = value;
      } else if (strcmp((const char *)cur_child->name, "active") == 0) {
        strcpy(rx_array_carrier->ru_carrier, (const char *)xmlNodeGetContent(cur_child));
      } else if (strcmp((const char *)cur_child->name, "downlink-radio-frame-offset") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->dl_radio_frame_offset = value;
      } else if (strcmp((const char *)cur_child->name, "downlink-sfn-offset") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->dl_sfn_offset = value;
      } else if (strcmp((const char *)cur_child->name, "gain-correction") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->gain_correction = value;
      } else if (strcmp((const char *)cur_child->name, "n-ta-offset") == 0) {
        value = atoi((const char *)xmlNodeGetContent(cur_child));
        rx_array_carrier->n_ta_offset = value;
      } 
    }
  }
}

static void find_uplane_conf_data(xmlNode *node, oai_oru_data_t *oru){
    for(xmlNode *cur_node = node; cur_node; cur_node = cur_node->next){
            if(cur_node->type == XML_ELEMENT_NODE){
            if(strcmp((const char*)cur_node->name, "tx-array-carriers") == 0){
                store_tx_array_carriers(cur_node->children, &oru->tx_array_carrier);
                break;
            } else {
                find_uplane_conf_data(cur_node->children, oru);
            }
        }
    }
    for(xmlNode *cur_node = node; cur_node; cur_node = cur_node->next){
            if(cur_node->type == XML_ELEMENT_NODE){
            if(strcmp((const char*)cur_node->name, "rx-array-carriers") == 0){
                store_rx_array_carriers(cur_node->children, &oru->rx_array_carrier);
                break;
            } else {
                find_uplane_conf_data(cur_node->children, oru);
            }
        }
    }
}

oai_oru_data_t get_uplane_conf_data(const char *filename){
  oai_oru_data_t oru = {0};

  // Initialize the xml file
  xmlDoc *doc = xmlReadFile(filename, NULL, 0);
  xmlNode *root_element = xmlDocGetRootElement(doc);

  find_uplane_conf_data(root_element->children, &oru);

  return oru;
}

int main() {
    // Define and initialize Tx and Rx array carrier structures

    char *filename = "192.168.4.34_conf.xml";
    // oai_oru_data_t oru = {
    //     .tx_array_carrier = {
    //         .name = "txarraycarrier0",
    //         .arfcn_center = 646668,
    //         .center_channel_bw = 3700020000, 
    //         .channel_bw = 100000000,
    //         .ru_carrier = "ACTIVE",
    //         .rw_duplex_scheme = "TDD",
    //         .rw_type = "NR",
    //         .gain = 27.0,
    //         .dl_radio_frame_offset = 0,
    //         .dl_sfn_offset = 0
    //     },
    //     .rx_array_carrier = {
    //         .name = "rxarraycarrier0",
    //         .arfcn_center = 646668,
    //         .center_channel_bw = 3700020000, 
    //         .channel_bw = 100000000,
    //         .ru_carrier = "ACTIVE",
    //         .dl_radio_frame_offset = 0,
    //         .dl_sfn_offset = 0,
    //         .gain_correction = 0.0,
    //         .n_ta_offset = 25600
    //     }
    // };
    oai_oru_data_t oru = {0};

    // Read the uplane configurations from a file and populate oru
    oru = get_uplane_conf_data(filename);

    // Generate the XML file
    char *xml_string = generate_uplane_conf_xml(&oru);

    // Print the XML string
    printf("%s\n", xml_string);

    return 0;
}
