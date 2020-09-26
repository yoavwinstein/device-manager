export type PropertyValueType = null | string | Number;
export type DeviceType = {
  deviceName: string;
  problem: Number;
  properties: Record<string, PropertyValueType>;
  children: Array<DeviceType>;
};
