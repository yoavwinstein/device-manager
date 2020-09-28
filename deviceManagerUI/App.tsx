/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * Generated with the TypeScript template
 * https://github.com/react-native-community/react-native-template-typescript
 *
 * @format
 */

import React from 'react';
import {
  SafeAreaView,
  StyleSheet,
  ScrollView,
  View,
  Text,
  StatusBar,
  CheckBox,
  TextInput,
  Image,
} from 'react-native';
import TreeView from 'react-native-final-tree-view';
import {NativeModules, NativeEventEmitter} from 'react-native';
import {DeviceType, PropertyValueType} from './Device';
import {DevicePropertiesView} from './DevicePropertiesView';

const NativeModuleEventEmitter = new NativeEventEmitter(
  NativeModules.NativeModule,
);
NativeModules.NativeModule.init();

function fixTree(tree: any) {
  tree.id = tree.properties.DEVPKEY_Device_InstanceId;
  tree.children.forEach((element: any) => {
    fixTree(element);
  });
}

function getIndicator(isExpanded, hasChildrenNodes) {
  if (!hasChildrenNodes) {
    return <Text> </Text>;
  } else if (isExpanded) {
    return <Text> </Text>;
  } else {
    return <Text> </Text>;
  }
  // return <Image source={require('./images/jacob.png')} />;
}

const App = () => {
  const [inputState, setInputState] = React.useState('');
  const [selectedDevice, setSelectedDevice] = React.useState<DeviceType | null>(
    null,
  );
  const [rootDevice, setRootDevice] = React.useState<DeviceType | null>(null);
  const [showDevicesWithProblems, setShowDevicesWithProblems] = React.useState(
    false,
  );

  function deviceMatchesFilter(device: any) {
    if (selectedDevice) {
      return selectedDevice === device;
    }
    if (showDevicesWithProblems) {
      // A special case for "Problem"
      return device.problem != 0;
    }
    for (const [key, value] of Object.entries(device.properties)) {
      if (key.includes(inputState) || String(value).includes(inputState)) {
        return true;
      }
    }
    return false;
  }

  function addDeviceToMap(device: any, map: any) {
    let matchFilterRecursive = deviceMatchesFilter(device);
    device.displayChildren = [];
    device.children.forEach((childDevice: any) => {
      let shouldDisplayDevice = addDeviceToMap(childDevice, map);
      if (shouldDisplayDevice) {
        device.displayChildren.push(childDevice);
      }
      matchFilterRecursive = shouldDisplayDevice || matchFilterRecursive;
    });

    map[device.properties.DEVPKEY_Device_InstanceId] = matchFilterRecursive;
    return matchFilterRecursive;
  }

  function onFileActivated(fileContent: string) {
    let parsedRootDevice: DeviceType = JSON.parse(fileContent);
    setSelectedDevice(null);
    setRootDevice(parsedRootDevice);
  }

  React.useEffect(() => {
    NativeModuleEventEmitter.addListener('onFileActivated', onFileActivated);
    return () => {
      NativeModuleEventEmitter.removeListener(
        'onFileActivated',
        onFileActivated,
      );
    };
  });

  let map: Record<string, boolean> = {};
  if (rootDevice !== null) {
    rootDevice.deviceName = 'Root Device'; // Root device is the only device which doesn't have a name, or so it seems
    fixTree(rootDevice);
    addDeviceToMap(rootDevice, map);
  }

  let optionalDeviceProperties =
    selectedDevice === null ? (
      <></>
    ) : (
      <DevicePropertiesView device={selectedDevice} />
    );
  return (
    <>
      <StatusBar barStyle="dark-content" />
      <View style={{flexDirection: 'row'}}>
        <TextInput
          style={{width: 300}}
          value={inputState}
          onChangeText={(str) => {
            setInputState(str);
          }}
        />
        <Text>Show only devices with problems: </Text>
        <CheckBox
          value={showDevicesWithProblems}
          onValueChange={(val) => {
            setShowDevicesWithProblems(val);
          }}
        />
      </View>
      <SafeAreaView>
        <ScrollView contentInsetAdjustmentBehavior="automatic">
          <View>
            <TreeView<DeviceType>
              data={rootDevice === null ? [] : [rootDevice]}
              childrenKey="displayChildren"
              isNodeExpanded={(id: DeviceType) => {
                // This is probably a bug in react-native-final-tree-view - id is actually a string representing the .key member of DeviceType
                let idReal = (id as unknown) as string;
                return map[idReal];
              }}
              onNodePress={({node, level}) => {
                if (selectedDevice === node) {
                  setSelectedDevice(null);
                } else {
                  setSelectedDevice(node);
                }
              }}
              renderNode={({node, level, isExpanded, hasChildrenNodes}) => {
                return (
                  <View
                    style={{
                      flexDirection: 'row',
                      marginLeft: 25 * level,
                      backgroundColor:
                        node === selectedDevice ? 'cyan' : 'default',
                    }}>
                    {getIndicator(isExpanded, hasChildrenNodes)}
                    <Text
                      style={{
                        fontSize: 14,
                        color: node.problem ? 'red' : 'black',
                      }}>
                      {node.deviceName}
                    </Text>
                  </View>
                );
              }}
            />
            {optionalDeviceProperties}
          </View>
        </ScrollView>
      </SafeAreaView>
    </>
  );
};

export default App;
