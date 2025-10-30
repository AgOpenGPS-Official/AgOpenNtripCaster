using AgOpenNtripCaster.Server.Data;
using AgOpenNtripCaster.Server.Models.DTOs;
using AgOpenNtripCaster.Server.Models.Entities;
using Microsoft.EntityFrameworkCore;

namespace AgOpenNtripCaster.Server.Services.Configuration;

public interface INetworkInfoService
{
    Task<NetworkInfoDto?> GetNetworkInfoAsync();
    Task<NetworkInfoDto> UpdateNetworkInfoAsync(UpdateNetworkInfoRequest request);
}

public class NetworkInfoService : INetworkInfoService
{
    private readonly ApplicationDbContext _context;
    private readonly ILogger<NetworkInfoService> _logger;

    public NetworkInfoService(ApplicationDbContext context, ILogger<NetworkInfoService> logger)
    {
        _context = context;
        _logger = logger;
    }

    public async Task<NetworkInfoDto?> GetNetworkInfoAsync()
    {
        try
        {
            var networkInfo = await _context.NetworkInfos.FirstOrDefaultAsync();

            if (networkInfo == null)
            {
                _logger.LogWarning("No NetworkInfo found in database");
                return null;
            }

            return MapToDto(networkInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error getting network info");
            throw;
        }
    }

    public async Task<NetworkInfoDto> UpdateNetworkInfoAsync(UpdateNetworkInfoRequest request)
    {
        try
        {
            // Convert string values to char
            var authRequired = string.IsNullOrWhiteSpace(request.AuthenticationRequired)
                ? 'Y'
                : request.AuthenticationRequired[0];
            var feeRequired = string.IsNullOrWhiteSpace(request.FeeRequired)
                ? 'N'
                : request.FeeRequired[0];

            // Get existing or create new
            var networkInfo = await _context.NetworkInfos.FirstOrDefaultAsync();

            if (networkInfo == null)
            {
                // Create new
                networkInfo = new NetworkInfo
                {
                    Identifier = request.Identifier,
                    Operator = request.Operator,
                    AuthenticationRequired = authRequired,
                    FeeRequired = feeRequired,
                    Website = request.Website,
                    Email = request.Email,
                    StartDate = request.StartDate,
                    EndDate = request.EndDate,
                    UpdatedAt = DateTime.UtcNow
                };

                _context.NetworkInfos.Add(networkInfo);
                _logger.LogInformation("Created new NetworkInfo: {Identifier}", request.Identifier);
            }
            else
            {
                // Update existing
                networkInfo.Identifier = request.Identifier;
                networkInfo.Operator = request.Operator;
                networkInfo.AuthenticationRequired = authRequired;
                networkInfo.FeeRequired = feeRequired;
                networkInfo.Website = request.Website;
                networkInfo.Email = request.Email;
                networkInfo.StartDate = request.StartDate;
                networkInfo.EndDate = request.EndDate;
                networkInfo.UpdatedAt = DateTime.UtcNow;

                _context.NetworkInfos.Update(networkInfo);
                _logger.LogInformation("Updated NetworkInfo: {Identifier}", request.Identifier);
            }

            await _context.SaveChangesAsync();
            return MapToDto(networkInfo);
        }
        catch (Exception ex)
        {
            _logger.LogError(ex, "Error updating network info");
            throw;
        }
    }

    private static NetworkInfoDto MapToDto(NetworkInfo entity)
    {
        return new NetworkInfoDto
        {
            Id = entity.Id,
            Identifier = entity.Identifier,
            Operator = entity.Operator,
            AuthenticationRequired = entity.AuthenticationRequired,
            FeeRequired = entity.FeeRequired,
            Website = entity.Website,
            Email = entity.Email,
            StartDate = entity.StartDate,
            EndDate = entity.EndDate,
            UpdatedAt = entity.UpdatedAt
        };
    }
}
