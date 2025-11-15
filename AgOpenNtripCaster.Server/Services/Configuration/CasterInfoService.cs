using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Configuration;

public interface ICasterInfoService
{
    Task<CasterInfoDto?> GetCasterInfoAsync();
    Task<CasterInfoDto> UpdateCasterInfoAsync(UpdateCasterInfoRequest request);
}

public class CasterInfoService : ICasterInfoService
{
    private readonly ApplicationDbContext _context;
    private readonly ILogger<CasterInfoService> _logger;

    public CasterInfoService(ApplicationDbContext context, ILogger<CasterInfoService> logger)
    {
        _context = context;
        _logger = logger;
    }

    public async Task<CasterInfoDto?> GetCasterInfoAsync()
    {
        try
        {
            var casterInfo = await _context.CasterInfos.FirstOrDefaultAsync();

            if (casterInfo == null)
            {
                _logger.LogWarning("No CasterInfo found in database");
                return null;
            }

            return MapToDto(casterInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting caster info");
            throw;
        }
    }

    public async Task<CasterInfoDto> UpdateCasterInfoAsync(UpdateCasterInfoRequest request)
    {
        try
        {
            // Get existing or create new
            var casterInfo = await _context.CasterInfos.FirstOrDefaultAsync();

            if (casterInfo == null)
            {
                // Create new
                casterInfo = new CasterInfo
                {
                    Identifier = request.Identifier,
                    Operator = request.Operator,
                    NmeaSupport = request.NmeaSupport,
                    Country = request.Country,
                    Latitude = request.Latitude,
                    Longitude = request.Longitude,
                    FallbackHost = request.FallbackHost,
                    Port = request.Port,
                    Description = request.Description,
                    DefaultNetwork = request.DefaultNetwork,
                    DefaultCountryCode = request.DefaultCountryCode,
                    DefaultGenerator = request.DefaultGenerator,
                    DefaultCompression = request.DefaultCompression,
                    DefaultAuthentication = request.DefaultAuthentication,
                    DefaultFeeRequired = request.DefaultFeeRequired,
                    UpdatedAt = DateTime.UtcNow
                };

                _context.CasterInfos.Add(casterInfo);
                _logger.LogInformation("Created new CasterInfo: {Identifier}", request.Identifier);
            }
            else
            {
                // Update existing
                casterInfo.Identifier = request.Identifier;
                casterInfo.Operator = request.Operator;
                casterInfo.NmeaSupport = request.NmeaSupport;
                casterInfo.Country = request.Country;
                casterInfo.Latitude = request.Latitude;
                casterInfo.Longitude = request.Longitude;
                casterInfo.FallbackHost = request.FallbackHost;
                casterInfo.Port = request.Port;
                casterInfo.Description = request.Description;
                casterInfo.DefaultNetwork = request.DefaultNetwork;
                casterInfo.DefaultCountryCode = request.DefaultCountryCode;
                casterInfo.DefaultGenerator = request.DefaultGenerator;
                casterInfo.DefaultCompression = request.DefaultCompression;
                casterInfo.DefaultAuthentication = request.DefaultAuthentication;
                casterInfo.DefaultFeeRequired = request.DefaultFeeRequired;
                casterInfo.UpdatedAt = DateTime.UtcNow;

                _context.CasterInfos.Update(casterInfo);
                _logger.LogInformation("Updated CasterInfo: {Identifier}", request.Identifier);
            }

            await _context.SaveChangesAsync();
            return MapToDto(casterInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating caster info");
            throw;
        }
    }

    private static CasterInfoDto MapToDto(CasterInfo entity)
    {
        return new CasterInfoDto
        {
            Id = entity.Id,
            Identifier = entity.Identifier,
            Operator = entity.Operator,
            NmeaSupport = entity.NmeaSupport,
            Country = entity.Country,
            Latitude = entity.Latitude,
            Longitude = entity.Longitude,
            FallbackHost = entity.FallbackHost,
            Port = entity.Port,
            Description = entity.Description,
            DefaultNetwork = entity.DefaultNetwork,
            DefaultCountryCode = entity.DefaultCountryCode,
            DefaultGenerator = entity.DefaultGenerator,
            DefaultCompression = entity.DefaultCompression,
            DefaultAuthentication = entity.DefaultAuthentication,
            DefaultFeeRequired = entity.DefaultFeeRequired,
            UpdatedAt = entity.UpdatedAt
        };
    }
}
