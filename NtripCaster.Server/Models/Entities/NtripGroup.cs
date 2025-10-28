namespace NtripCaster.Server.Models.Entities;

public class NtripGroup
{
    public int Id { get; set; }
    public string Name { get; set; } = string.Empty;
    public string Description { get; set; } = string.Empty;
    public DateTime CreatedAt { get; set; } = DateTime.UtcNow;
    public bool IsActive { get; set; } = true;

    // Relations
    public ICollection<NtripUser> Users { get; set; } = new List<NtripUser>();
    public ICollection<MountPoint> MountPoints { get; set; } = new List<MountPoint>();
}
