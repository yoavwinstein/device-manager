import React from 'react';
import {DeviceType, PropertyValueType} from './Device';

// TODO
type DeviceTreeViewProps = {
  rootDevice: DeviceType;
  selectedDevice: DeviceType | null;
  setSelectedDevice: React.SetStateAction<DeviceType | null>;
};

export function DeviceTreeView({
  rootDevice,
  selectedDevice,
  setSelectedDevice,
}: DeviceTreeViewProps) {}
