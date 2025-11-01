using System.Net.Http.Headers;
using System.Net.Http.Json;
using System.Text.Json;

namespace NtripCaster.LoadTest;

/// <summary>
/// API DTOs for authentication and mount point management
/// </summary>
public class LoginRequest
{
    public string Email { get; set; } = string.Empty;
    public string Password { get; set; } = string.Empty;
}

public class LoginResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public string AccessToken { get; set; } = string.Empty;
    public string RefreshToken { get; set; } = string.Empty;
}

public class CreateMountPointRequest
{
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string SourcePassword { get; set; } = string.Empty;
    public bool RequireClientAuthentication { get; set; } = true;
    public bool IsActive { get; set; } = true;
    public List<int> AllowedGroupIds { get; set; } = new();
    public decimal? Latitude { get; set; } = 52.0m;
    public decimal? Longitude { get; set; } = 5.0m;
}

public class CreateMountPointResponse
{
    public bool Success { get; set; }
    public string Message { get; set; } = string.Empty;
    public int MountPointId { get; set; }
}

public class MountPointDto
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public string SourcePassword { get; set; } = string.Empty;
}

/// <summary>
/// HTTP client for NtripCaster API operations
/// Handles authentication and mount point management
/// </summary>
public class ApiClient
{
    private readonly string _apiBaseUrl;
    private readonly HttpClient _httpClient;
    private string? _accessToken;

    public string? AccessToken => _accessToken;

    public ApiClient(string apiBaseUrl)
    {
        _apiBaseUrl = apiBaseUrl.TrimEnd('/');
        _httpClient = new HttpClient();
    }

    /// <summary>
    /// Login with email and password to get JWT token
    /// </summary>
    public async Task<bool> LoginAsync(string email, string password, CancellationToken cancellationToken = default)
    {
        try
        {
            var request = new LoginRequest { Email = email, Password = password };
            var response = await _httpClient.PostAsJsonAsync(
                $"{_apiBaseUrl}/api/auth/login",
                request,
                cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                Console.WriteLine($"❌ Login failed: {response.StatusCode}");
                var content = await response.Content.ReadAsStringAsync(cancellationToken);
                Console.WriteLine($"   Response: {content}");
                return false;
            }

            var result = await response.Content.ReadAsAsync<LoginResponse>(cancellationToken);
            if (result == null || !result.Success)
            {
                Console.WriteLine($"❌ Login failed: {result?.Message ?? "Unknown error"}");
                return false;
            }

            _accessToken = result!.AccessToken;
            _httpClient.DefaultRequestHeaders.Authorization = new AuthenticationHeaderValue("Bearer", _accessToken);
            return true;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Login error: {ex.Message}");
            return false;
        }
    }

    /// <summary>
    /// Create a mount point for testing
    /// </summary>
    public async Task<int?> CreateMountPointAsync(string name, string sourcePassword, string description = "", CancellationToken cancellationToken = default)
    {
        try
        {
            if (string.IsNullOrEmpty(_accessToken))
            {
                Console.WriteLine("❌ Not authenticated. Call LoginAsync first.");
                return null;
            }

            var request = new CreateMountPointRequest
            {
                Name = name,
                Description = description,
                SourcePassword = sourcePassword,
                RequireClientAuthentication = true,
                IsActive = true,
                Latitude = 52.0m,
                Longitude = 5.0m
            };

            var response = await _httpClient.PostAsJsonAsync(
                $"{_apiBaseUrl}/api/mountpoints",
                request,
                cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                Console.WriteLine($"❌ Failed to create mount point {name}: {response.StatusCode}");
                var content = await response.Content.ReadAsStringAsync(cancellationToken);
                Console.WriteLine($"   Response: {content}");
                return null;
            }

            var result = await response.Content.ReadAsAsync<CreateMountPointResponse>(cancellationToken);
            if (result == null || !result.Success)
            {
                Console.WriteLine($"❌ Failed to create mount point {name}: {result?.Message ?? "Unknown error"}");
                return null;
            }

            return result!.MountPointId;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Error creating mount point {name}: {ex.Message}");
            return null;
        }
    }

    /// <summary>
    /// Delete a mount point
    /// </summary>
    public async Task<bool> DeleteMountPointAsync(int mountPointId, CancellationToken cancellationToken = default)
    {
        try
        {
            if (string.IsNullOrEmpty(_accessToken))
            {
                Console.WriteLine("❌ Not authenticated. Call LoginAsync first.");
                return false;
            }

            var response = await _httpClient.DeleteAsync(
                $"{_apiBaseUrl}/api/mountpoints/{mountPointId}",
                cancellationToken);

            if (!response.IsSuccessStatusCode)
            {
                Console.WriteLine($"⚠️  Failed to delete mount point {mountPointId}: {response.StatusCode}");
                return false;
            }

            return true;
        }
        catch (Exception ex)
        {
            Console.WriteLine($"⚠️  Error deleting mount point {mountPointId}: {ex.Message}");
            return false;
        }
    }
}

/// <summary>
/// Extension method for reading JSON from HttpContent
/// </summary>
public static class HttpContentExtensions
{
    public static async Task<T?> ReadAsAsync<T>(this HttpContent content, CancellationToken cancellationToken = default)
    {
        var json = await content.ReadAsStringAsync(cancellationToken);
        return JsonSerializer.Deserialize<T>(json, new JsonSerializerOptions { PropertyNameCaseInsensitive = true });
    }
}
