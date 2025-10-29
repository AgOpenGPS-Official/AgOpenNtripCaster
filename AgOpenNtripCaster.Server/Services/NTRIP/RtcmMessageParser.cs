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

            // Extract message type (first 12 bits of payload)
            int messageType = ((data[3] & 0xFC) >> 2);

            if (messageType != 1005)
                return null;

            // RTCM 1005 has specific structure, but exact parsing requires bit-level manipulation
            // For now, return a placeholder that will be enriched with actual parsing
            var message = new Rtcm1005Message
            {
                MessageType = 1005,
                ReferenceStationId = ExtractBits(data, 12, 12),
                Latitude = ExtractSignedBits(data, 110, 34) / 10_000_000.0m,  // in 1/10^7 degrees
                Longitude = ExtractSignedBits(data, 144, 35) / 10_000_000.0m, // in 1/10^7 degrees
                Height = ExtractSignedBits(data, 179, 22) / 10_000.0m,         // in mm
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
