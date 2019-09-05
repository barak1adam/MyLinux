/*********************************************************************
 *  File: svrrp.c
 *    The file contains code for VRRP service
 ********************************************************************/

/* linux h-files */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

/* uid h-files */
#include "uid_utils.h"
#include "uid_vrrp.h"
#include "uid_sys.h"
#include "uid_if.h"
#include "uid_rtr.h"
/* nbase h-files */
#include "gen_log.h"
#include "gen_sys.h"
#include "vrrp_tbl.h"
#include "vrrp.h"
#include "ui_private.h"
#include "ui_system.h"
#include "osr-features.h"

extern pthread_mutex_t vrrp_msg_mtx;

/* recieve state notifications from keepalived (vrrp daemon) on this socket. this is the server side of the socket */
static UID_CONNECTION_T           vrrp_server_connection = NULL;
static SOCK_INFO_T    vrrp_sockets[1];

extern void sigterm_handler(int sig_num);

#if HAVE_VRRP

/* declare SPF handlers */
static REQ_FUNC_T VRRP_vr_mng;
static REQ_FUNC_T VRRP_if_mng;

/* array of requests */
static REQ_INFO_T req_array[] = {

    /* VRRP requests */
    { UID_VRRP_SET_VR,                  &VRRP_vr_mng, 0 },
    { UID_VRRP_DEL_VR,                  &VRRP_vr_mng, 0 },
    { UID_VRRP_GET_VR,                  &VRRP_vr_mng, 0 },
    { UID_VRRP_UPDATE_STATE_VR,         &VRRP_vr_mng, 0 },
    { UID_VRRP_GET_NEXT_VR,             &VRRP_vr_mng, 0 },
    { UID_VRRP_GET_NEXT_VR_ON_IF,       &VRRP_vr_mng, 0 },
    { UID_VRRP_START_VR,                &VRRP_vr_mng, 0 },
    { UID_VRRP_STOP_VR,                 &VRRP_vr_mng, 0 },
    { UID_VRRP_RESTART_VR,              &VRRP_vr_mng, 0 },
    { UID_VRRP_SET_IF,                  &VRRP_if_mng, 0 },
    { UID_VRRP_DEL_IF,                  &VRRP_if_mng, 0 },
    { UID_VRRP_GET_IF,                  &VRRP_if_mng, 0 },
    { UID_VRRP_GET_NEXT_IF,             &VRRP_if_mng, 0 },
    { UID_VRRP_ENABLE_IF,               &VRRP_if_mng, 0 },
    { UID_VRRP_DISABLE_IF,              &VRRP_if_mng, 0 },
    { UID_VRRP_STOP_IF,                 &VRRP_if_mng, 0 },
};

/* This callback gets control when something comes onto the socket from vrrp_notify */
static UID_STATUS_T _rx_state_notification (IN int sock, IN char *buffer, IN int len, IN void *additional_param)
{
  size_t                  msg_size;
  int                     rc;

  msg_size = sizeof (UID_VRRP_VR_T);
  GEN_LOG_log_console(" _rx_state_notification\n\r");

  rc = UID_read_foreign_socket_msg ("Vrrp State Notification", sock, buffer, len, msg_size);
  if (rc != OK) {
    return rc;
  }
  /*rc = vrrp_update_state_vr((UID_VRRP_VR_T*)buffer);
  if (rc != OK) {
    return rc;
  }*/
  return OK;

}

static void
svrrp_event_handler(IN EVENT_TYPE_T event_type,
		    IN void *event_param,
		    IN int event_param_size,
		    IN void *addiatinal_param)
{
  UID_IF_EVENT_T        *if_event;
  UI_IF_T               *ui_if_ptr;
  UID_VRRP_IF_T         uid_vrrp_if;
  STATUS                status;

  switch (event_type) {
  case VIF_EVNT_MNG_CHANGE:
    GEN_LOG_log_console("VIF_EVNT_MNG_CHANGE for IP ADDDDDDDD\n\r");
    if_event = (UID_IF_EVENT_T *)event_param;
    ui_if_ptr = &if_event->interface;

    /*   We need to process only LAN ethernet interface, except default forwarding interface   */
    if ((UI_IF_TYPE_LAN_ETHER != ui_if_ptr->type) || ui_if_ptr->aliasf ||
        (0 == strcmp (ui_if_ptr->dev, DEF_FWD_ETHER_DEVICE_NAME))) {
      return;
    }
    
    GEN_LOG_log_console("ui_if_ptr->dev = %s\n\r", ui_if_ptr->dev);
    memset(&uid_vrrp_if.device, 0, sizeof (uid_vrrp_if.device));
    memcpy (uid_vrrp_if.device, ui_if_ptr->dev, sizeof (uid_vrrp_if.device));
    GEN_LOG_log_console("uid_vrrp_if.device = %s\n\r", uid_vrrp_if.device);
    uid_vrrp_if.field_mask = UID_VRRP_IF_MASK_ENABLE;

    switch (if_event->type) {
     case UID_IF_EVENT_IF_IP_DEL:
      GEN_LOG_log_console(" UID_IF_EVENT_IF_IP_DEL LLLLLLLLLL\n\r");
#if 1
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	    if (OK != (status = vrrp_disable_if (ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Activate VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	}
#endif      
      break;
     case UID_IF_EVENT_IF_IP_ADD:
      GEN_LOG_log_console(" UID_IF_EVENT_IF_IP_ADD EEEEEEEEEE\n\r");
#if 1
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	    if (OK != (status = vrrp_enable_if (ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Activate VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	}
#endif      
#if 0
      //GEN_LOG_log_console ("%s: Try to restart VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	if (UI_IF_ENTRY_STATE_UP == ui_if_ptr->state) {
	  if (VRRP_IF_ENABLE == uid_vrrp_if.enable) {
	    GEN_LOG_log (INFO_MODE, "%s: Try to activate VRRP IF %s", __FUNCTION__, ui_if_ptr->dev);
	    GEN_LOG_log_console ("%s: Try to activate VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	    if (OK != (status = vrrp_enable_if_ex (ui_if_ptr->dev, VRRP_IF_ACTIVE))) {
	      GEN_LOG_log (ERR_MODE, "%s: Activate VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	  } else if (VRRP_IF_ACTIVE == uid_vrrp_if.enable) {
	    GEN_LOG_log_console ("%s: Try to restart VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	    if (OK != (status = vrrp_restart_if (ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Restart VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	  }
	  else
	    GEN_LOG_log_console(" UID_IF_EVENT_IF_IP_ADD Elseeeee\n\r");
	}
      }
#endif
      break;
     default:
      GEN_LOG_log (DEBUG_MODE, "VIF_EVNT_VIF_CHANGE: %s event_type = %d", ui_if_ptr->dev, (int)if_event->type);
      break;
    }
    break;
  case VIF_EVNT_VIF_CHANGE:
    GEN_LOG_log_console("VIF_EVNT_VIF_CHANGE\n\r");
    GEN_LOG_log(DEBUG_MODE, "VIF_EVNT_VIF_CHANGE");
    if_event = (UID_IF_EVENT_T *)event_param;
    ui_if_ptr = &if_event->interface;

    /*   We need to process only LAN ethernet interface, except default forwarding interface   */
    if ((UI_IF_TYPE_LAN_ETHER != ui_if_ptr->type) || ui_if_ptr->aliasf ||
        (0 == strcmp (ui_if_ptr->dev, DEF_FWD_ETHER_DEVICE_NAME))) {
      return;
    }

    memcpy (uid_vrrp_if.device, ui_if_ptr->dev, sizeof (uid_vrrp_if.device));
    uid_vrrp_if.field_mask = UID_VRRP_IF_MASK_ENABLE;



    /* handle adding or removal of secondaries */
    switch (if_event->type) {
    case UID_IF_EVENT_IF_ADDED:
      GEN_LOG_log_console("%s: UID_IF_EVENT_IF_ADDED, state of intf: %d\n\r",  ui_if_ptr->dev, ui_if_ptr->state);
      GEN_LOG_log (INFO_MODE, "%s: Activate VRRP IF %s", __FUNCTION__, ui_if_ptr->dev);
      GEN_LOG_log (DEBUG_MODE, "VIF_EVNT_VIF_CHANGE: UID_IF_EVENT_IF_ADDED %s", ui_if_ptr->dev);
      if (UI_IF_ENTRY_STATE_UP == ui_if_ptr->state) {
	if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	  GEN_LOG_log (INFO_MODE, "%s: Activate VRRP IF %s", __FUNCTION__, ui_if_ptr->dev);
	  GEN_LOG_log_console ("%s: Activate VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	  if (OK != (status = vrrp_enable_if_ex (ui_if_ptr->dev, VRRP_IF_ACTIVE))) {
	    GEN_LOG_log (ERR_MODE, "%s: Activate VRRP IF %s failed (status = %d)",
			 __FUNCTION__, ui_if_ptr->dev, status);
	  }
	} else if (UID_VRRP_IF_NOT_FOUND != status) {
	  GEN_LOG_log (ERR_MODE, "VIF_EVNT_VIF_CHANGE: UID_IF_EVENT_IF_ADDED %s: vrrp_get_if error = %d",
		       ui_if_ptr->dev, status);
	}
      }
      break;
    case UID_IF_EVENT_IF_MODIFIED:
    case UID_IF_EVENT_IF_IPv6_ADD:
    case UID_IF_EVENT_IF_IPv6_DEL:
      GEN_LOG_log (DEBUG_MODE, "VIF_EVNT_VIF_CHANGE: UID_IF_EVENT_IF_MODIFIED %s", ui_if_ptr->dev);
      GEN_LOG_log_console("%s: UID_IF_EVENT_IF_MODIFIED\n\r",  ui_if_ptr->dev);
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	if (UI_IF_ENTRY_STATE_UP == ui_if_ptr->state) {
	  GEN_LOG_log_console("%s: UID_IF_EVENT_IF_MODIFIED UI_IF_ENTRY_STATE_UP = ui_if_ptr->state\n\r",  ui_if_ptr->dev);
	}
#if 0
	GEN_LOG_log_console("%s: UID_VRRP_DISABLE_IF\n",  ui_if_ptr->dev);
	if (OK != (status = vrrp_disable_if(ui_if_ptr->dev))) {
		GEN_LOG_log (ERR_MODE, "%s: Disable VRRP IF %s failed (status = %d)",
			     __FUNCTION__, ui_if_ptr->dev, status);
	}
	
	sleep(1);


	GEN_LOG_log_console("%s: UID_VRRP_ENABLE_IF\n",  ui_if_ptr->dev);
	if (OK != (status = vrrp_enable_if(ui_if_ptr->dev))) {
		GEN_LOG_log (ERR_MODE, "%s: ENABLE VRRP IF %s failed (status = %d)",
			     __FUNCTION__, ui_if_ptr->dev, status);
	}
#endif      
      }
#if 0
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	if (UI_IF_ENTRY_STATE_UP == ui_if_ptr->state) {
	  GEN_LOG_log_console("%s: UID_IF_EVENT_IF_MODIFIED UI_IF_ENTRY_STATE_UP = ui_if_ptr->state\n\r",  ui_if_ptr->dev);
	  if (VRRP_IF_ENABLE == uid_vrrp_if.enable) {
	    GEN_LOG_log (INFO_MODE, "%s: Try to activate VRRP IF %s", __FUNCTION__, ui_if_ptr->dev);
	    GEN_LOG_log_console ("%s: Try to activate VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	    if (OK != (status = vrrp_enable_if_ex (ui_if_ptr->dev, VRRP_IF_ACTIVE))) {
	      GEN_LOG_log (ERR_MODE, "%s: Activate VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	  } else if (VRRP_IF_ACTIVE == uid_vrrp_if.enable) {
	    GEN_LOG_log_console ("%s: Try to restart VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	    if (OK != (status = vrrp_restart_if (ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Restart VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
	  }
	}
      }
#endif
#if 0
      GEN_LOG_log_console("%s: UID_VRRP_DISABLE_IF\n",  ui_if_ptr->dev);
      if (OK != (status = vrrp_disable_if(ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Disable VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
      }
      /*unsigned int usecs = 50000;
      if (OK != (status = (STATUS)usleep(usecs)))
	GEN_LOG_log(ERR_MODE, "%s: failed to call usleep perror: %s status = %d", __FUNCTION__, perror, status);*/
      sleep(1);


      GEN_LOG_log_console("%s: UID_VRRP_ENABLE_IF\n",  ui_if_ptr->dev);
      if (OK != (status = vrrp_enable_if(ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: ENABLE VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
      }
      /* In case IP of VIF is changed, restart all VRs to recheck for ownership */
      if (OK != (status = vrrp_restart_if (ui_if_ptr->dev))) {
	  GEN_LOG_log (ERR_MODE, "%s: Restart VRRP IF %s failed (status = %d)",
		       __FUNCTION__, ui_if_ptr->dev, status);
	}
#endif      
#if 0      
      GEN_LOG_log_console ("%s: Try to restart VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
      if (OK == (status = vrrp_get_if (&uid_vrrp_if))) {
	    GEN_LOG_log_console ("%s: Try to restart VRRP IF %s\n\r", __FUNCTION__, ui_if_ptr->dev);
	    if (OK != (status = vrrp_restart_if (ui_if_ptr->dev))) {
	      GEN_LOG_log (ERR_MODE, "%s: Restart VRRP IF %s failed (status = %d)",
			   __FUNCTION__, ui_if_ptr->dev, status);
	    }
      }
#endif      
      break;
    case UID_IF_EVENT_IF_STATE_CHANGED:
      GEN_LOG_log_console("%s: UID_IF_EVENT_IF_STATE_CHANGED\n\r",  ui_if_ptr->dev);
      break;
    case UID_IF_EVENT_IF_DELETED:
      /*   DO NOTHING.
           When interface is deleted/disabled SIF sends UiD request UID_VRRP_DEL_IF / UID_VRRP_STOP_IF   */
      break;
    default:
      GEN_LOG_log (DEBUG_MODE, "VIF_EVNT_VIF_CHANGE: %s event_type = %d", ui_if_ptr->dev, (int)if_event->type);
      break;
    }
    break;

  default:
    GEN_LOG_log(DEBUG_MODE, "svrrp_event_handler: invalid event # %d", (int) event_type);
    break;
  }
}  /* svrrp_event_handler() */

static void svrrp_event_cb_post_init (EVENT_TYPE_T event_type, void *event_param,
  int event_param_size, void *additional_dummy)
{

  if (event_type != SYS_EVNT_POST_INIT)
    return;

  vrrp_post_init();
}

static EVNT_INFO_T svrrp_events[] =
{
  {VIF_EVNT_VIF_CHANGE, &svrrp_event_handler, NULL},
  {VIF_EVNT_MNG_CHANGE, &svrrp_event_handler, NULL},
  {SYS_EVNT_POST_INIT,  &svrrp_event_cb_post_init, NULL},
};

static UID_STATUS_T
VRRP_vr_mng(IN UID_CONNECTION_T   conn,
	    INOUT char            *buff/*[MAX_REQ_BUFF_SIZE]*/)
{
  UID_VRRP_VR_T *uid_vr_ptr;
  STATUS status;
  unsigned long  required_mask;
  int    op;
  int    size;

  if (sizeof (UID_VRRP_VR_T) > GET_UID_BODY_SIZE(buff)) {
    return UID_REQ_INVALID_MESSAGE_LENGTH;
  }
  uid_vr_ptr = (UID_VRRP_VR_T *)UID_GET_BODY_MSG(buff);

  /* check required fields (key) which are common to all requests */
  required_mask = (UID_VRRP_VR_MASK_DEV | UID_VRRP_VR_MASK_VRID);
  if ((uid_vr_ptr->field_mask & required_mask) != required_mask) {
      return UID_REQ_INVALID_PARAMETER;
  }
  op = GET_S_UID_OPER(buff);

  pthread_mutex_lock(&vrrp_msg_mtx);

  switch(op){
  case UID_VRRP_SET_VR:
      /* calculate the exact length using the 'naddr' field in the object */
      if ((uid_vr_ptr->field_mask & UID_VRRP_VR_MASK_NADDR) &&
	  (((sizeof (UID_VRRP_VR_T) - 1) + (uid_vr_ptr->naddr * sizeof(IP_COMBO))) !=
	  GET_UID_BODY_SIZE(buff)))
	  return UID_REQ_INVALID_MESSAGE_LENGTH;
      if (OK != (status = vrrp_set_vr(uid_vr_ptr))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_UPDATE_STATE_VR:
      if (OK != (status = vrrp_update_state_vr(uid_vr_ptr))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_GET_VR:
      /* NOTICE: vrrp_get_vr() writes 'size' bytes to the buffer,
	 and 'size' may be bigger than GET_UID_BODY_SIZE(buff),
	 which is the size of the request.
	 However, this should be OK, because buffer is a MAX_REQ_BUFF_SIZE
	 bytes array, and we are not supposed to override this limit */
      if (OK != (status = vrrp_get_vr(uid_vr_ptr, &size))) {
	  if (UID_VRRP_VR_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
	  SET_UID_BODY_SIZE(buff, 0);
      }
      else
	  SET_UID_BODY_SIZE(buff, size);
      break;
  case UID_VRRP_GET_NEXT_VR:
  case UID_VRRP_GET_NEXT_VR_ON_IF:
      /* NOTICE: vrrp_get_next_vr() writes 'size' bytes to the buffer,
	 and 'size' may be bigger than GET_UID_BODY_SIZE(buff),
	 which is the size of the request.
	 However, this should be OK, because buffer is a MAX_REQ_BUFF_SIZE
	 bytes array, and we are not supposed to override this limit */
      if (OK != (status = vrrp_get_next_vr(uid_vr_ptr, &size, (UID_VRRP_GET_NEXT_VR == op) ? TRUE : FALSE))) {
	  SET_UID_BODY_SIZE(buff, 0);
      }
      else
	  SET_UID_BODY_SIZE(buff, size);
      break;
  case UID_VRRP_DEL_VR:
      if (OK != (status = vrrp_del_vr(uid_vr_ptr->dev, uid_vr_ptr->vrid))) {
	  if (UID_VRRP_VR_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_START_VR:
      if (OK != (status = vrrp_enable_vr(uid_vr_ptr->dev, uid_vr_ptr->vrid))) {
	  if (UID_VRRP_VR_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_STOP_VR:
      if (OK != (status = vrrp_disable_vr(uid_vr_ptr->dev, uid_vr_ptr->vrid))) {
	  if (UID_VRRP_VR_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_RESTART_VR:
      if (OK != (status = vrrp_restart_vr(uid_vr_ptr->dev, uid_vr_ptr->vrid))) {
	  if (UID_VRRP_VR_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
    default:
      SET_UID_BODY_SIZE(buff, 0);
      pthread_mutex_unlock(&vrrp_msg_mtx);
      return UID_REQ_COMMAND_HAS_NOT_BEEN_IMPLEMENTED;
  }
  
  pthread_mutex_unlock(&vrrp_msg_mtx);
  return status;
}

static UID_STATUS_T
VRRP_if_mng(IN UID_CONNECTION_T   conn,
	    INOUT char            *buff/*[MAX_REQ_BUFF_SIZE]*/)
{
  UID_VRRP_IF_T *uid_if_ptr;
  STATUS status;
  unsigned long  required_mask;
  int    op;

  if (sizeof (UID_VRRP_IF_T) != GET_UID_BODY_SIZE(buff)) {
    return UID_REQ_INVALID_MESSAGE_LENGTH;
  }
  uid_if_ptr = (UID_VRRP_IF_T *)UID_GET_BODY_MSG(buff);

  /* check required fields (key) which are common to all requests */
  required_mask = UID_VRRP_IF_MASK_DEVICE;
  if ((uid_if_ptr->field_mask & required_mask) != required_mask) {
      return UID_REQ_INVALID_PARAMETER;
  }
  op = GET_S_UID_OPER(buff);
  
  pthread_mutex_lock(&vrrp_msg_mtx);

  switch(op){
  case UID_VRRP_SET_IF:
      if (OK != (status = vrrp_set_if(uid_if_ptr))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_GET_IF:
      if (OK != (status = vrrp_get_if(uid_if_ptr))) {
	  if (UID_VRRP_IF_NOT_FOUND != status)
	      ERR_MSG(get_err_str(status));
	  SET_UID_BODY_SIZE(buff, 0);
      }
      else
	  SET_UID_BODY_SIZE(buff, sizeof(*uid_if_ptr));
      break;
  case UID_VRRP_GET_NEXT_IF:
      if (OK != (status = vrrp_get_next_if(uid_if_ptr))) {
	  SET_UID_BODY_SIZE(buff, 0);
      }
      else
	  SET_UID_BODY_SIZE(buff, sizeof(*uid_if_ptr));
      break;
  case UID_VRRP_DEL_IF:
      if (OK != (status = vrrp_del_if(uid_if_ptr->device))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_ENABLE_IF:
      GEN_LOG_log_console("%s: UID_VRRP_ENABLE_IF\n",  uid_if_ptr->device);
      GEN_LOG_log_console ("%s: Try to activate VRRP IF %s\n\r", __FUNCTION__, uid_if_ptr->device);
      if (OK != (status = vrrp_enable_if(uid_if_ptr->device))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_DISABLE_IF:
      GEN_LOG_log_console("%s: UID_VRRP_DISABLE_IF\n",  uid_if_ptr->device);
      if (OK != (status = vrrp_disable_if(uid_if_ptr->device))) {
	  ERR_MSG(get_err_str(status));
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
  case UID_VRRP_STOP_IF:
      GEN_LOG_log_console("%s: UID_VRRP_STOP_IF\n",  uid_if_ptr->device);
      if (OK != (status = vrrp_stop_if(uid_if_ptr->device))) {
	if (UID_VRRP_IF_NOT_FOUND != status) {
	  ERR_MSG(get_err_str(status));
	} else {
	  DEBUG_MSG(get_err_str(status));
	}
      }
      SET_UID_BODY_SIZE(buff, 0);
      break;
    default:
      SET_UID_BODY_SIZE(buff, 0);
      pthread_mutex_unlock(&vrrp_msg_mtx);
      return UID_REQ_COMMAND_HAS_NOT_BEEN_IMPLEMENTED;
  }

  pthread_mutex_unlock(&vrrp_msg_mtx);
  return status;
}

/* Signal wrapper. */
static void *
signal_set (int signo, void (*func)(int))
{
  int ret;
  struct sigaction sig;
  struct sigaction osig;

  sig.sa_handler = func;
  //sigemptyset (&sig.sa_mask);
  sigfillset (&sig.sa_mask);
  sig.sa_flags = 0;

  ret = sigaction (signo, &sig, &osig);

  if (ret < 0)
    return (SIG_ERR);
  else
    return (osig.sa_handler);
}

static
STATUS
signal_init()
{
    /* set signals handlers */
    if (signal_set (SIGTERM, sigterm_handler) == SIG_ERR) {
	ERR_MSG("Failed to set SIGTERM handler\n");
	return ERROR;
    }
    return OK;
}

int main (int argc, char *argv[])
{
    int                 rc = 0;
    size_t                        msg_size;
    STATUS status;

    /* initialize logging */
    if (GEN_LOG_default_init("svrrp") != OK) {
		ERR_MSG("Service \"svrrp\" failed to initialize logging system.");
    }

    if (OK != (status = signal_init())) {
		ERR_MSG("Failed to set signal handlers");
		return ERROR;
    }

    if (OK != UI_PRIVATE_init(NULL)) {
		DEBUG_MSG("Failed to initialize private configuration");
		return ERROR;
    }
    if (OK != UI_SYSTEM_init()) {
		DEBUG_MSG("Failed to initialize system configuration");
		return ERROR;
    }

    /*** socket intialization - start here ***/

    /* create the socket for recieving state notifications of virtual routers */
    rc = UiD_OpenSocketConnection (UID_VRRP_NOTIFY_PATH, &vrrp_server_connection);
    if (OK != rc) {
      GEN_LOG_log (FATAL_MODE, "can't create VRRP server notifications socket:%s\n", get_internal_err_str (rc));
    }
    if (OK == UiD_enlarge_connection (vrrp_server_connection, UID_VRRP_NOTIFY_PATH, 1)) {
      GEN_LOG_log(INFO_MODE, "%s:vrrp server socket has been enlarged", __FUNCTION__); 
    }

    msg_size = sizeof (UID_VRRP_VR_T);

    rc = UiD_create_server_foreign_socket (UID_VRRP_NOTIFY_PATH, _rx_state_notification, msg_size, &vrrp_sockets[0]);
    if (OK != rc) {
      GEN_LOG_log (ERR_MODE, "Can't create VRRP notifications connection (error=%d)", rc);
    }

    /*** socket initialization - end here ***/

    if (OK != (status = vrrp_init())) {
		ERR_MSG_ARG("Failed to init vrrp module (error=%d)\n", status);
		return 1;
    }

    DEBUG_MSG("svrrp server started.");

    /* call to UiD dispatcher */
    rc = UiD_dispatcher_ex(req_array, sizeof(req_array)/sizeof(REQ_INFO_T),
			   svrrp_events, sizeof(svrrp_events)/sizeof(EVNT_INFO_T),
			   vrrp_sockets, sizeof (vrrp_sockets) / sizeof (SOCK_INFO_T));

    vrrp_cleanup();
    GEN_LOG_release();
    return rc;
}

#endif /* HAVE_VRRP */
