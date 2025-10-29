using Microsoft.AspNetCore.Identity;
using Microsoft.AspNetCore.Identity.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore;
using NtripCaster.Server.Models.Entities;

namespace AgOpenNtripCaster.Server.Data;

public class ApplicationDbContext : IdentityDbContext<NtripUser>
{
    public ApplicationDbContext(DbContextOptions<ApplicationDbContext> options)
        : base(options)
    {
    }

    // DbSets
    public DbSet<NtripGroup> NtripGroups { get; set; } = null!;
    public DbSet<MountPoint> MountPoints { get; set; } = null!;
    public DbSet<ClientSession> ClientSessions { get; set; } = null!;
    public DbSet<SourceConnection> SourceConnections { get; set; } = null!;

    protected override void OnModelCreating(ModelBuilder builder)
    {
        base.OnModelCreating(builder);

        // Configure NTRIP user
        builder.Entity<NtripUser>()
            .HasMany(u => u.Groups)
            .WithMany(g => g.Users)
            .UsingEntity(j => j.ToTable("UserGroups"));

        builder.Entity<NtripUser>()
            .HasMany(u => u.ClientSessions)
            .WithOne(c => c.User)
            .HasForeignKey(c => c.UserId)
            .OnDelete(DeleteBehavior.SetNull);

        // Configure NtripGroup
        builder.Entity<NtripGroup>()
            .HasMany(g => g.MountPoints)
            .WithMany(m => m.AllowedGroups)
            .UsingEntity(j => j.ToTable("GroupMountPoints"));

        // Configure MountPoint
        builder.Entity<MountPoint>()
            .HasMany(m => m.ClientSessions)
            .WithOne(c => c.MountPoint)
            .HasForeignKey(c => c.MountPointId)
            .OnDelete(DeleteBehavior.Cascade);

        builder.Entity<MountPoint>()
            .HasMany(m => m.SourceConnections)
            .WithOne(s => s.MountPoint)
            .HasForeignKey(s => s.MountPointId)
            .OnDelete(DeleteBehavior.Cascade);

        // Configure ClientSession
        builder.Entity<ClientSession>()
            .HasKey(c => c.Id);

        builder.Entity<ClientSession>()
            .HasOne(c => c.User)
            .WithMany(u => u.ClientSessions)
            .HasForeignKey(c => c.UserId)
            .OnDelete(DeleteBehavior.SetNull);

        // Configure SourceConnection
        builder.Entity<SourceConnection>()
            .HasOne(s => s.MountPoint)
            .WithMany(m => m.SourceConnections)
            .HasForeignKey(s => s.MountPointId)
            .OnDelete(DeleteBehavior.Cascade);
    }
}
