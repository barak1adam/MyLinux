#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* assignments */
static uint8_t
get_state_module_up(UID_MSG_MODULE_INSTANCE_T *instance,
	                           uint8_t *up, uint8_t *up2)
{
  int rc;
  char ip[64] = {}; /* same like {0} */

  if (!instance->state.module_power)
    return (*up = *up2 = FALSE);

  rc = md_ping(md_ip_str(instance->config.management_ip, ip));
  *up = *up2 = (rc == 0);

  return *up;

}


static int wget_nfv(const char *ip, const char *cmd, const char *outf, char* status)
{
    char exp[1024] = {};
    snprintf(exp, sizeof(exp) - 1, "wget -T 5 -t 1 -O %s http://%s:8888/%s >/dev/null 2>&1", outf, ip, cmd);

    int rc = system(exp);
    if (-1 == rc && ECHILD == errno) {
	    //sigchld_handler took the signal, no way to detect the real return code. Lets assume it was ok
	    rc = OK;
     }
     if (-1 == rc)
        strcpy(status, strerror(errno));
     else
        rc = WEXITSTATUS(rc);

     return rc;

}

int module_show_info_exec(UID_MSG_MODULE_INSTANCE_T *msg,
                          const module_type_t type, const char *name,
                          const Module_Request_Type_T req, FILE **fp,
                          Module_State_Type_T *state, char *status, ...)
{
    MODULED_CREDS_T     creds = {};
    int                 rc, module_up = TRUE, module_present = TRUE;
    int                 module_enabled = TRUE;
    va_list             args;

    /* best way to init a string to empty string which was already declared, make its length 0
     * set the first byte to null terminator which is '0', '\0' or 0 
     * BTW, using "" is possible only on declartion time (char *s = "";) or by strcpy... */
    status[0] = 0;

    /* check module state */
    rc = module_get_and_verify_state(msg, type, name, &module_present, &module_up,
                                     &module_enabled, &creds, state, status);
    if (OK != rc) {
        // state is filled
        return rc;
    }

    va_start(args, status);

    switch (type) {
        case module_type_lte:
            rc = module_lte_show_exec(req, &creds, fp, status, args);
            if (rc)
                *state = Module_State_GenericError;
            break;
        case module_type_vdsl:
            rc = module_vdsl_show_exec(req, &creds, fp, status, args);
            if (rc)
                *state = Module_State_GenericError;
            break;
        case module_type_nfv:
            rc = 0;
            break;
        default:
            strcpy(status, "Wrong module type");
            rc = -1;
            *state = Module_State_InvalidReq;
            break;
    }

    va_end(args);

    return rc;
}

