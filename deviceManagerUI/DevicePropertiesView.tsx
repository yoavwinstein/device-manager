import React from 'react';
import {DeviceType} from './Device';
import {TextInput} from 'react-native';

export type DevicePropertiesViewProps = {
  device: DeviceType;
};

export function DevicePropertiesView({device}: DevicePropertiesViewProps) {
  return (
    <TextInput multiline value={JSON.stringify(device.properties, null, 2)} />
  );
}
