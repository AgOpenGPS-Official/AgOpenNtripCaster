namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// RTCM message parser for extracting station coordinates and format detection
/// </summary>
public class RtcmMessageParser
{
    /// <summary>
    /// RTCM 1005 message structure (ARP (Antenna Reference Point))
    /// Provides precise position of reference station
    /// </summary>
    public class Rtcm1005Message
    {
        public int MessageType { get; set; } = 1005;
        public int ReferenceStationId { get; set; }
        public bool GpsIndicator { get; set; }
        public bool GlonassIndicator { get; set; }
        public bool GalileoIndicator { get; set; }
        public decimal Latitude { get; set; }
        public decimal Longitude { get; set; }
        public decimal Height { get; set; }
    }

    public class ParseResult
    {
        public string? DetectedFormat { get; set; }          // RTCM3, RTCM2.3, SPARTN, etc
        public string? DetectedNavSystems { get; set; }      // GPS, GLONASS, GALILEO, combined
        public Rtcm1005Message? Position1005 { get; set; }   // Position from 1005 message
    }

    /// <summary>
    /// Parse RTCM message and extract relevant information
    /// </summary>
    public static ParseResult ParseRtcmMessage(byte[] messageData)
    {
        var result = new ParseResult();

        if (messageData == null || messageData.Length < 3)
            return result;

        // Detect RTCM format by analyzing message structure
        result.DetectedFormat = DetectRtcmFormat(messageData);

        // If RTCM 3.x, try to parse position messages
        if (result.DetectedFormat?.StartsWith("RTCM3") == true)
        {
            result.Position1005 = TryParseRtcm1005(messageData);
            result.DetectedNavSystems = DetectNavigationSystems(messageData);
        }

        return result;
    }

    /// <summary>
    /// Detect RTCM format version from message bytes
    /// </summary>
    private static string? DetectRtcmFormat(byte[] data)
    {
        // RTCM 3.x starts with 0xD3 preamble
        if (data[0] == 0xD3)
        {
            // RTCM 3.x format
            return "RTCM3";
        }

        // RTCM 2.3 uses different format (not starting with 0xD3)
        if ((data[0] & 0xC0) == 0x00)
        {
            return "RTCM2.3";
        }

        // SPARTN format detection (Hexagon proprietary)
        if (data[0] == 0x73) // 's'
        {
            return "SPARTN";
        }

        return null;
    }

    /// <summary>
    /// Try to parse RTCM 1005 message (station position)
    /// Returns null if message is not 1005 or parsing fails
    /// </summary>
    private static Rtcm1005Message? TryParseRtcm1005(byte[] data)
    {
        try
        {
            // RTCM 3.x structure:
            // Byte 0: Preamble (0xD3)
            // Byte 1-2: Length (10 bits)
            // Byte 3+: Payload (6 bits per byte, bit-packed)

            if (data.Length < 6 || data[0] != 0xD3)
                return null;

            // Extract message type (first 12 bits of payload, starting at bit offset 0)
            // Byte 3 bits 7-2 (6 bits) + Byte 4 bits 7-6 (2 bits) = 8 bits... actually 12 bits total
            // Message type is bits 0-11 of the payload
            int messageType = ExtractBits(data, 24, 12);  // 24 bits offset = 3 bytes * 8

            if (messageType != 1005)
                return null;

            // RTCM 1005 structure:
            // Bits 12-23: Reference Station ID
            // Bits 24-26: System Indicators (GPS, GLONASS, Galileo)
            // Bits 27-64: ECEF-X (38 bits, in 0.0001m)
            // Bits 65-102: ECEF-Y (38 bits, in 0.0001m)
            // Bits 103-140: ECEF-Z (38 bits, in 0.0001m)
            // Bits 141-162: Height (22 bits, in 0.0001m)

            // Payload starts at bit 24 (after 3-byte RTCM3 header)
            // RTCM1005 structure (payload bits → frame bits, with +24 offset):
            // Bits 0-11 (frame 24-35): Message Type
            // Bits 12-23 (frame 36-47): Reference Station ID
            // Bits 24-26 (frame 48-50): System Indicators
            // Bit 27 (frame 51): Reserved
            // Bits 28-65 (frame 52-89): ECEF-X (38 bits)
            // Bits 66-103 (frame 90-127): ECEF-Y (38 bits)
            // Bits 104-141 (frame 128-165): ECEF-Z (38 bits)
            // Bits 142-163 (frame 166-187): Height (22 bits)

            int refStationId = ExtractBits(data, 12 + 24, 12);
            bool gpsIndicator = (ExtractBits(data, 24 + 24, 1) == 1);
            bool glonassIndicator = (ExtractBits(data, 25 + 24, 1) == 1);
            bool galileoIndicator = (ExtractBits(data, 26 + 24, 1) == 1);

            // Extract ECEF coordinates in 0.01m (centimeter) units
            long ecefXRaw = ExtractSignedBits(data, 28 + 24, 38);
            long ecefYRaw = ExtractSignedBits(data, 66 + 24, 38);
            long ecefZRaw = ExtractSignedBits(data, 104 + 24, 38);
            long heightRaw = ExtractSignedBits(data, 142 + 24, 22);

            double ecefX = ecefXRaw * 0.01;  // convert from centimeters to meters
            double ecefY = ecefYRaw * 0.01;  // convert from centimeters to meters
            double ecefZ = ecefZRaw * 0.01;  // convert from centimeters to meters
            double height = heightRaw * 0.01; // convert from centimeters to meters

            // DEBUG: Log raw message bytes and extracted values
            var hexString = string.Join(" ", data.Take(25).Select(b => $"{b:X2}"));
            System.Diagnostics.Debug.WriteLine($"RTCM1005 Message (first 25 bytes): {hexString}");
            System.Diagnostics.Debug.WriteLine($"RTCM1005 Raw - X:{ecefXRaw}, Y:{ecefYRaw}, Z:{ecefZRaw}, Height:{heightRaw}");
            System.Diagnostics.Debug.WriteLine($"RTCM1005 ECEF - X:{ecefX}, Y:{ecefY}, Z:{ecefZ}, Height:{height}");

            // Convert ECEF to Latitude/Longitude
            var (lat, lon) = ConvertEcefToLatLon(ecefX, ecefY, ecefZ);
            System.Diagnostics.Debug.WriteLine($"RTCM1005 Converted - Lat:{lat}, Lon:{lon}");

            // Expected for Netherlands: Lat ≈ 52.737, Lon ≈ 6.751, X ≈ 3880000, Y ≈ 900000, Z ≈ 5030000

            var message = new Rtcm1005Message
            {
                MessageType = 1005,
                ReferenceStationId = refStationId,
                GpsIndicator = gpsIndicator,
                GlonassIndicator = glonassIndicator,
                GalileoIndicator = galileoIndicator,
                Latitude = (decimal)lat,
                Longitude = (decimal)lon,
                Height = (decimal)height,
            };

            return message;
        }
        catch
        {
            return null;
        }
    }

    /// <summary>
    /// Detect which navigation systems are in the stream
    /// </summary>
    private static string? DetectNavigationSystems(byte[] data)
    {
        var systems = new List<string>();

        // Simple detection by looking for message types associated with each system
        // This is a simplified version - real implementation would need more sophisticated parsing

        // GPS messages: 1001-1009, 1071-1077
        // GLONASS messages: 1010-1012, 1081-1087
        // Galileo messages: 1020-1029, 1091-1097
        // BeiDou messages: 1040-1049, 1101-1107

        // For now, default to GPS
        systems.Add("GPS");

        // Could add detection logic here based on message type analysis
        // if (HasGlonassMessages(data)) systems.Add("GLONASS");
        // if (HasGalileoMessages(data)) systems.Add("GALILEO");

        return systems.Count > 0 ? string.Join("+", systems) : "GPS";
    }

    /// <summary>
    /// Convert ECEF (Earth-Centered Earth-Fixed) coordinates to Latitude/Longitude using WGS84
    /// </summary>
    private static (double latitude, double longitude) ConvertEcefToLatLon(double ecefX, double ecefY, double ecefZ)
    {
        // WGS84 parameters
        const double a = 6378137.0;           // Semi-major axis in meters
        const double e2 = 0.00669437999014132; // Eccentricity squared

        // Calculate longitude (simple: atan2(Y, X))
        double longitude = Math.Atan2(ecefY, ecefX) * 180.0 / Math.PI;

        // Calculate latitude using iterative method
        double p = Math.Sqrt(ecefX * ecefX + ecefY * ecefY);
        double latitude = Math.Atan2(ecefZ, p * (1 - e2));

        // Iterate to refine latitude
        for (int i = 0; i < 5; i++)
        {
            double N = a / Math.Sqrt(1 - e2 * Math.Sin(latitude) * Math.Sin(latitude));
            latitude = Math.Atan2(ecefZ + e2 * N * Math.Sin(latitude), p);
        }

        latitude = latitude * 180.0 / Math.PI;

        return (latitude, longitude);
    }

    /// <summary>
    /// Extract N bits from bit-packed data starting at bit offset
    /// </summary>
    private static int ExtractBits(byte[] data, int bitOffset, int bitCount)
    {
        int result = 0;

        for (int i = 0; i < bitCount; i++)
        {
            int byteIndex = (bitOffset + i) / 8;
            int bitIndex = 7 - ((bitOffset + i) % 8);

            if (byteIndex < data.Length)
            {
                int bit = (data[byteIndex] >> bitIndex) & 1;
                result = (result << 1) | bit;
            }
        }

        return result;
    }

    /// <summary>
    /// Extract signed N bits (two's complement) from bit-packed data
    /// </summary>
    private static long ExtractSignedBits(byte[] data, int bitOffset, int bitCount)
    {
        long result = ExtractBits(data, bitOffset, bitCount);

        // Check sign bit
        if ((result & (1L << (bitCount - 1))) != 0)
        {
            // Negative number - apply two's complement
            result -= (1L << bitCount);
        }

        return result;
    }
}
