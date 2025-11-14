namespace AgOpenNtripCaster.Server.Services.NTRIP;

/// <summary>
/// RTCM message parser for extracting station coordinates and format detection
/// </summary>
public class RtcmMessageParser
{
    /// <summary>
    /// RTCM 1005 message - Stationary RTK Reference Station ARP (Antenna Reference Point)
    /// Provides precise position of reference station antenna
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
        public decimal Height { get; set; }  // Height from ECEF-Z (approximate)
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
            int messageType = (int)ExtractBits(data, 24, 12);  // 24 bits offset = 3 bytes * 8

            if (messageType != 1005)
                return null;

            // RTCM 1005 structure (from pyrtcm reference implementation):
            // Message payload bits (0-based from start of payload, after 3-byte header):
            // Bits 0-11:    DF002 - Message Number (1005) - 12 bits
            // Bits 12-23:   DF003 - Reference Station ID - 12 bits
            // Bits 24-29:   DF021 - ITRF Realization Year - 6 bits
            // Bit 30:       DF022 - GPS Indicator - 1 bit
            // Bit 31:       DF023 - GLONASS Indicator - 1 bit
            // Bit 32:       DF024 - Galileo Indicator - 1 bit
            // Bit 33:       DF141 - Reference-Station Indicator - 1 bit
            // Bits 34-71:   DF025 - Antenna Ref Point ECEF-X (38 bits, signed)
            // Bit 72:       DF142 - Single Receiver Oscillator Indicator - 1 bit
            // Bit 73:       DF001_1 - Reserved - 1 bit
            // Bits 74-111:  DF026 - Antenna Ref Point ECEF-Y (38 bits, signed)
            // Bit 112-113:  DF364 - Quarter Cycle Indicator - 2 bits
            // Bits 114-151: DF027 - Antenna Ref Point ECEF-Z (38 bits, signed)

            // All bit offsets are from start of payload (byte 3 in the full message)
            // Payload bit 0 = byte 3, bit 7 (MSB)
            // We need to add 24 to convert to byte array bit offset

            int refStationId = (int)ExtractBits(data, 24 + 12, 12);

            // Indicators are at bits 30-32 of payload
            bool gpsIndicator = (ExtractBits(data, 24 + 30, 1) == 1);
            bool glonassIndicator = (ExtractBits(data, 24 + 31, 1) == 1);
            bool galileoIndicator = (ExtractBits(data, 24 + 32, 1) == 1);

            // ECEF coordinates are NOT consecutive - they have single-bit fields between them!
            long ecefXRaw = ExtractSignedBits(data, 24 + 34, 38);   // Bit 34-71
            long ecefYRaw = ExtractSignedBits(data, 24 + 74, 38);   // Bit 74-111 (after 2 single-bit fields)
            long ecefZRaw = ExtractSignedBits(data, 24 + 114, 38);  // Bit 114-151 (after 2-bit quarter cycle)

            // Convert from 0.0001m to meters
            double ecefX = ecefXRaw * 0.0001;
            double ecefY = ecefYRaw * 0.0001;
            double ecefZ = ecefZRaw * 0.0001;

            // Convert ECEF to Latitude/Longitude
            var (lat, lon) = ConvertEcefToLatLon(ecefX, ecefY, ecefZ);

            return new Rtcm1005Message
            {
                MessageType = 1005,
                ReferenceStationId = refStationId,
                GpsIndicator = gpsIndicator,
                GlonassIndicator = glonassIndicator,
                GalileoIndicator = galileoIndicator,
                Latitude = (decimal)lat,
                Longitude = (decimal)lon,
                Height = 0  // RTCM 1005 doesn't provide antenna height (see 1006 for height)
            };
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
    /// Bit offset 0 = first bit after the 3-byte RTCM header
    /// </summary>
    private static ulong ExtractBits(byte[] data, int bitOffset, int bitCount)
    {
        ulong result = 0;

        for (int i = 0; i < bitCount; i++)
        {
            int byteIndex = (bitOffset + i) / 8;
            int bitIndex = 7 - ((bitOffset + i) % 8);

            if (byteIndex < data.Length)
            {
                uint bit = (uint)((data[byteIndex] >> bitIndex) & 1);
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
        ulong unsignedValue = ExtractBits(data, bitOffset, bitCount);

        // Check sign bit (MSB)
        if ((unsignedValue & (1UL << (bitCount - 1))) != 0)
        {
            // Negative number - apply two's complement
            // Create a mask of all 1s, then shift left by bitCount to get sign extension
            ulong mask = ~0UL << bitCount;  // All 1s shifted left
            return (long)(unsignedValue | mask);
        }

        return (long)unsignedValue;
    }
}
