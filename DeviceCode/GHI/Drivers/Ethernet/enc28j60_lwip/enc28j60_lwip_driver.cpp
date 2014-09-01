#include <tinyhal.h>
#include "net_decl_lwip.h"
#include "enc28j60_lwip.h"

extern "C"
{
#include "netif\etharp.h"
#include "lwip\dhcp.h"
#include "lwip\tcpip.h"
#include "lwip\dns.h"
}

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata = "g_ENC28J60_LWIP_Driver"
#endif

static ENC28J60_LWIP_Driver g_ENC28J60_LWIP_Driver;
static struct netif         g_ENC28J60_NetIF;

HAL_CONTINUATION    InterruptTaskContinuation;
HAL_COMPLETION      LwipUpTimeCompletion;
static UINT32       LwipNetworkStatus = 0;
static UINT32       LwipLastIpAddress = 0;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section zidata
#endif

extern ENC28J60_LWIP_DEVICE_CONFIG   g_ENC28J60_LWIP_Config;
extern NETWORK_CONFIG                g_NetworkConfig;

extern unsigned short enc28j60_lwip_read_phy_register(SPI_CONFIGURATION *spiConf, UINT8 registerAddress);



void enc28j60_status_callback(struct netif *netif)
{
    if(LwipLastIpAddress != netif->ip_addr.addr)
    {
        Network_PostEvent( NETWORK_EVENT_TYPE_ADDRESS_CHANGED, 0 );
        LwipLastIpAddress = netif->ip_addr.addr;
    }

#if !defined(BUILD_RTM)
    lcd_printf("\f\n\n\n\n\n\nLink Update: %s\n", (netif_is_up(netif) ? "UP  " : "DOWN") );
    lcd_printf("        IP: %d.%d.%d.%d\n", (netif->ip_addr.addr >>  0) & 0xFF, 
                                             (netif->ip_addr.addr >>  8) & 0xFF,
                                             (netif->ip_addr.addr >> 16) & 0xFF,
                                             (netif->ip_addr.addr >> 24) & 0xFF);
    lcd_printf("        SM: %d.%d.%d.%d\n", (netif->netmask.addr >>  0) & 0xFF, 
                                             (netif->netmask.addr >>  8) & 0xFF,
                                             (netif->netmask.addr >> 16) & 0xFF,
                                             (netif->netmask.addr >> 24) & 0xFF);    
    lcd_printf("        GW: %d.%d.%d.%d\n", (netif->gw.addr >>  0) & 0xFF, 
                                             (netif->gw.addr >>  8) & 0xFF,
                                             (netif->gw.addr >> 16) & 0xFF,
                                             (netif->gw.addr >> 24) & 0xFF);
    debug_printf("IP Address: %d.%d.%d.%d\n", (netif->ip_addr.addr >>  0) & 0xFF, 
                                             (netif->ip_addr.addr >>  8) & 0xFF,
                                             (netif->ip_addr.addr >> 16) & 0xFF,
                                             (netif->ip_addr.addr >> 24) & 0xFF);
#if LWIP_DNS
    if(netif->flags & NETIF_FLAG_DHCP)
    {
        struct ip_addr dns1 = dns_getserver(0);
        struct ip_addr dns2 = dns_getserver(1);
        
        lcd_printf("         dns1: %d.%d.%d.%d\n",(dns1.addr >>  0) & 0xFF, 
                                                  (dns1.addr >>  8) & 0xFF,
                                                  (dns1.addr >> 16) & 0xFF,
                                                  (dns1.addr >> 24) & 0xFF);
        
        lcd_printf("         dns2: %d.%d.%d.%d\n",(dns2.addr >>  0) & 0xFF, 
                                                  (dns2.addr >>  8) & 0xFF,
                                                  (dns2.addr >> 16) & 0xFF,
                                                  (dns2.addr >> 24) & 0xFF);
    }
#endif
#endif
}

err_t   enc28j60_ethhw_init( netif * myNetIf) 
{ 
    myNetIf->mtu = ETHERSIZE;

    /* ethhw_init() is user-defined */
    /* use ip_input instead of ethernet_input for non-ethernet hardware */
    /* (this function is assigned to netif.input and should be called by the hardware driver) */

    /* Assign the xmit routine to the stack's netif and call the driver's Open */
    myNetIf->output = etharp_output;
    myNetIf->linkoutput = enc28j60_lwip_xmit;
    myNetIf->status_callback = enc28j60_status_callback;

    enc28j60_lwip_open( myNetIf );

    return 0; 
}

void lwip_interrupt_continuation( )
{
    NATIVE_PROFILE_PAL_NETWORK();
    GLOBAL_LOCK(irq);
    
    if(!InterruptTaskContinuation.IsLinked())
    {
        InterruptTaskContinuation.Enqueue();
    }
}

void lwip_network_uptime_completion(void *arg)
{
    NATIVE_PROFILE_PAL_NETWORK();

    UINT16 phyStat = 0;
    
    {
        GLOBAL_LOCK(irq);

        phyStat = enc28j60_lwip_read_phy_register(&g_ENC28J60_LWIP_Config.DeviceConfigs[0].SPI_Config, ENC28J60_PHSTAT2);
    }

    // linkstatus bit
    UINT32 status = ((phyStat >> ENC28J60_PHSTAT2_LSTAT_BIT) & 1);

    if(status != LwipNetworkStatus)
    {
        struct netif* pNetIf = (struct netif*)arg;
        
        if(status)
        {
            tcpip_timeout(1000, (sys_timeout_handler)netif_set_link_up, (void*)pNetIf);
            tcpip_timeout(1000, (sys_timeout_handler)netif_set_up, (void*)pNetIf);

            Network_PostEvent( NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, NETWORK_EVENT_FLAGS_IS_AVAILABLE );
        }
        else
        {
            tcpip_callback((sys_timeout_handler)netif_set_link_down, (void*)pNetIf);
            tcpip_callback((sys_timeout_handler)netif_set_down, (void*)pNetIf);

            Network_PostEvent( NETWORK_EVENT_TYPE__AVAILABILITY_CHANGED, 0);
        }

        Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
        Events_Set(SYSTEM_EVENT_FLAG_NETWORK);

        LwipNetworkStatus = status;
    }

    LwipUpTimeCompletion.EnqueueDelta64( 2000000 );
}

void InitContinuations( struct netif* pNetIf )
{
    InterruptTaskContinuation.InitializeCallback( (HAL_CALLBACK_FPN)enc28j60_lwip_interrupt, &g_ENC28J60_NetIF );

    LwipUpTimeCompletion.InitializeForUserMode( (HAL_CALLBACK_FPN)lwip_network_uptime_completion, pNetIf );
    
    LwipUpTimeCompletion.EnqueueDelta64( 2000000 );
}

BOOL Network_Interface_Bind(int index)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if(index >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs)) return FALSE;

    return g_ENC28J60_LWIP_Driver.Bind(&g_ENC28J60_LWIP_Config.DeviceConfigs[index], index);
}
int  Network_Interface_Open(int index)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if(index >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs)) return -1;
    
    HAL_CONFIG_BLOCK::ApplyConfig( ENC28J60_LWIP_DEVICE_CONFIG::GetDriverName(), &g_ENC28J60_LWIP_Config, sizeof(g_ENC28J60_LWIP_Config) );
    
    return g_ENC28J60_LWIP_Driver.Open(&g_ENC28J60_LWIP_Config.DeviceConfigs[index], index);
}
BOOL Network_Interface_Close(int index)
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();
    if(index >= ARRAYSIZE(g_ENC28J60_LWIP_Config.DeviceConfigs)) return FALSE;

    return g_ENC28J60_LWIP_Driver.Close(&g_ENC28J60_LWIP_Config.DeviceConfigs[index], index);
}

int ENC28J60_LWIP_Driver::Open( ENC28J60_LWIP_DRIVER_CONFIG* config, int index )
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    /* Network interface variables */
    struct ip_addr ipaddr, netmask, gw;
    struct netif *pNetIF;
    int len;
    const SOCK_NetworkConfiguration *iface;
    iface = &g_NetworkConfig.NetworkInterfaces[index];

    if(0 == (iface->flags & SOCK_NETWORKCONFIGURATION_FLAGS_DHCP))
    {
        ipaddr.addr  = iface->ipaddr;
        gw.addr      = iface->gateway;
        netmask.addr = iface->subnetmask;
    }
    else
    {
        /* Set network address variables - this will be set by either DHCP or when the configuration is applied */
        IP4_ADDR(&gw     ,   0,   0,   0, 0);
        IP4_ADDR(&ipaddr ,   0,   0,   0, 0);
        IP4_ADDR(&netmask, 255, 255, 255, 0);
    }

    if(config == NULL) return -1;

    

    len = g_ENC28J60_NetIF.hwaddr_len;

    if(len == 0 || iface->macAddressLen < len)
    {
        len = iface->macAddressLen;
        g_ENC28J60_NetIF.hwaddr_len = len;
    }
   
    memcpy(g_ENC28J60_NetIF.hwaddr, iface->macAddressBuffer, len);

    g_ENC28J60_NetIF.flags = NETIF_FLAG_IGMP | NETIF_FLAG_BROADCAST;
        
    pNetIF = netif_add( &g_ENC28J60_NetIF, &ipaddr, &netmask, &gw, NULL, enc28j60_ethhw_init, ethernet_input );
       
    /* Enable the INTERRUPT pin */                            
    if (CPU_GPIO_EnableInputPin2(config->INT_Pin, 
                                 FALSE,                                                         /* Glitch filter enable */
                                 (GPIO_INTERRUPT_SERVICE_ROUTINE) &enc28j60_lwip_pre_interrupt, /* ISR                  */
                                 &g_ENC28J60_NetIF,                                             /* minor number         */
                                 GPIO_INT_EDGE_LOW ,                                            /* Interrupt edge       */
                                 RESISTOR_PULLUP) == FALSE)                                     /* Resistor State       */
    {
        return -1;
    }

    netif_set_default( pNetIF );
    LwipNetworkStatus = 1;
    netif_set_link_up( pNetIF );
    netif_set_up( pNetIF );
    
    /* Enable the CHIP SELECT pin */
    if (CPU_GPIO_EnableInputPin (config->SPI_Config.DeviceCS, 
                            FALSE, 
                            NULL,
                            GPIO_INT_NONE,
                            RESISTOR_PULLUP) == FALSE)
    {
        return -1;                          
    }
    
    /* Initialize the continuation routine for the driver interrupt and receive */    
    InitContinuations( pNetIF );
    
    return g_ENC28J60_NetIF.num;
    
}

BOOL ENC28J60_LWIP_Driver::Close( ENC28J60_LWIP_DRIVER_CONFIG* config, int index )
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    if(config == NULL) return FALSE;

    LwipUpTimeCompletion.Abort();

    netif_set_down( &g_ENC28J60_NetIF );
    netif_remove( &g_ENC28J60_NetIF );

    /* Disable the INTERRUPT pin */                            
    CPU_GPIO_EnableInputPin2(config->INT_Pin, 
                              FALSE,                         /* Glitch filter enable */
                              NULL,                          /* ISR                  */
                              0,                             /* minor number         */
                              GPIO_INT_NONE,                 /* Interrupt edge       */
                              RESISTOR_PULLUP);              /* Resistor State       */

    InterruptTaskContinuation.Abort();

    LwipNetworkStatus = 0;
    
    enc28j60_lwip_close( &g_ENC28J60_NetIF );

    memset(&g_ENC28J60_NetIF, 0, sizeof(g_ENC28J60_NetIF));

    return TRUE;

}

BOOL  ENC28J60_LWIP_Driver::Bind  ( ENC28J60_LWIP_DRIVER_CONFIG* config, int index )
{
    NATIVE_PROFILE_HAL_DRIVERS_ETHERNET();

    return TRUE;    
}

    


