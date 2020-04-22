/**
 * @file srvpm.h
 * @brief srv:pm service.
 */
#pragma once

#include "types.h"

/// Initializes srv:pm and the service API.
Result srvPmInit(Handle* srvPmHandle, Handle* srvHandle);

/// Exits srv:pm and the service API.
void srvPmExit(Handle* srvPmHandle, Handle* srvHandle);

/**
 * @brief Publishes a notification to a process.
 * @param notificationId ID of the notification.
 * @param process Process to publish to.
 */
Result SRVPM_PublishToProcess(const Handle* srvPmHandle, u32 notificationId, Handle process);

/**
 * @brief Publishes a notification to all processes.
 * @param notificationId ID of the notification.
 */
Result SRVPM_PublishToAll(const Handle* srvPmHandle, u32 notificationId);

/**
 * @brief Registers a process with SRV.
 * @param pid ID of the process.
 * @param count Number of services within the service access control data.
 * @param serviceAccessControlList Service Access Control list.
 */
Result SRVPM_RegisterProcess(const Handle* srvPmHandle, u32 pid, u32 count, const char (*serviceAccessControlList)[8]);

/**
 * @brief Unregisters a process with SRV.
 * @param pid ID of the process.
 */
Result SRVPM_UnregisterProcess(const Handle* srvPmHandle, u32 pid);
