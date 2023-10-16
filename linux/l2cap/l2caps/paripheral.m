#import <Foundation/Foundation.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface PeripheralManagerDelegate : NSObject <CBPeripheralManagerDelegate>
@property (strong, nonatomic) CBPeripheralManager *peripheralManager;
@property (strong, nonatomic) CBMutableCharacteristic *l2capCharacteristic;
@end

@implementation PeripheralManagerDelegate

- (instancetype)init {
    self = [super init];
    if (self) {
        self.peripheralManager = [[CBPeripheralManager alloc] initWithDelegate:self queue:nil];
    }
    return self;
}

- (void)peripheralManagerDidUpdateState:(CBPeripheralManager *)peripheral {
    if (peripheral.state == CBManagerStatePoweredOn) {
        CBUUID *serviceUUID = [CBUUID UUIDWithString:@"3C8DC645-7BB0-4F18-8A2A-1413E75D63F6"];
        self.l2capCharacteristic = [[CBMutableCharacteristic alloc]
                                   initWithType:[CBUUID UUIDWithString:@"1B10E3C4-E651-4E4D-ACFE-387D2F7A18BC"]
                                   properties:CBCharacteristicPropertyRead
                                        value:nil
                                  permissions:CBAttributePermissionsReadable];

        CBMutableService *customService = [[CBMutableService alloc] initWithType:serviceUUID primary:YES];
        [customService setCharacteristics:@[self.l2capCharacteristic]];
        [self.peripheralManager addService:customService];

        NSDictionary *advertisingData = @{
            CBAdvertisementDataLocalNameKey: @"MyBLEServer",
            CBAdvertisementDataServiceUUIDsKey: @[serviceUUID]
        };
        [self.peripheralManager startAdvertising:advertisingData];
    }
}

- (void)peripheralManager:(CBPeripheralManager *)peripheral central:(CBCentral *)central didUnsubscribeFromCharacteristic:(CBCharacteristic *)characteristic {
    if ([characteristic.UUID isEqual:self.l2capCharacteristic.UUID]) {
        // Send a "Hello, Device B!" message
        NSString *message = @"Hello, Device B!";
        NSData *data = [message dataUsingEncoding:NSUTF8StringEncoding];
        [self.peripheralManager updateValue:data forCharacteristic:self.l2capCharacteristic onSubscribedCentrals:nil];
    }
}

@end

int main(int argc, const char *argv[]) {
    @autoreleasepool {
        PeripheralManagerDelegate *delegate = [[PeripheralManagerDelegate alloc] init];
        [[NSRunLoop currentRunLoop] run];
    }
    return 0;
}

