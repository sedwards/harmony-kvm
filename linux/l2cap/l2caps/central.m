#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface CentralManagerDelegate : NSObject <CBCentralManagerDelegate, CBPeripheralDelegate>
@property (strong, nonatomic) CBCentralManager *centralManager;
@property (strong, nonatomic) CBPeripheral *peripheral;
@end

@implementation CentralManagerDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        self.centralManager = [[CBCentralManager alloc] initWithDelegate:self queue:nil];
    }
    return self;
}

- (void)centralManagerDidUpdateState:(CBCentralManager *)central {
    if (central.state == CBManagerStatePoweredOn) {
        [self.centralManager scanForPeripheralsWithServices:@[[CBUUID UUIDWithString:@"3C8DC645-7BB0-4F18-8A2A-1413E75D63F6"]] options:nil];
    }
}

- (void)centralManager:(CBCentralManager *)central didDiscoverPeripheral:(CBPeripheral *)peripheral advertisementData:(NSDictionary<NSString *,id> *)advertisementData RSSI:(NSNumber *)RSSI {
    // Connect to the peripheral
    self.peripheral = peripheral;
    [central connectPeripheral:self.peripheral options:nil];
}

- (void)centralManager:(CBCentralManager *)central didConnectPeripheral:(CBPeripheral *)peripheral {
    [self.peripheral setDelegate:self];
    [self.peripheral discoverServices:@[[CBUUID UUIDWithString:@"3C8DC645-7BB0-4F18-8A2A-1413E75D63F6"]]];
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverServices:(NSError *)error {
    for (CBService *service in peripheral.services) {
        if ([service.UUID isEqual:[CBUUID UUIDWithString:@"3C8DC645-7BB0-4F18-8A2A-1413E75D63F6"]]) {
            [peripheral discoverCharacteristics:@[[CBUUID UUIDWithString:@"1B10E3C4-E651-4E4D-ACFE-387D2F7A18BC"]] forService:service];
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didDiscoverCharacteristicsForService:(CBService *)service error:(NSError *)error {
    for (CBCharacteristic *characteristic in service.characteristics) {
        if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:@"1B10E3C4-E651-4E4D-ACFE-387D2F7A18BC"]]) {
            [peripheral readValueForCharacteristic:characteristic];
        }
    }
}

- (void)peripheral:(CBPeripheral *)peripheral didUpdateValueForCharacteristic:(CBCharacteristic *)characteristic error:(NSError *)error {
    if ([characteristic.UUID isEqual:[CBUUID UUIDWithString:@"1B10E3C4-E651-4E4D-ACFE-387D2F7A18BC"]]) {
        NSString *receivedData = [[NSString alloc] initWithData:characteristic.value encoding:NSUTF8StringEncoding];
        NSLog(@"Received data from Device A: %@", receivedData);
    }
}

@end

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        CentralManagerDelegate *delegate = [[CentralManagerDelegate alloc] init];
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}

