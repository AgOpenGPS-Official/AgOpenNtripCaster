using System;
using Microsoft.EntityFrameworkCore.Migrations;
using Npgsql.EntityFrameworkCore.PostgreSQL.Metadata;

#nullable disable

namespace AgOpenNtripCaster.Server.Data.Migrations
{
    /// <inheritdoc />
    public partial class AddPerformanceMetrics : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "PerformanceMetrics",
                columns: table => new
                {
                    Id = table.Column<int>(type: "integer", nullable: false)
                        .Annotation("Npgsql:ValueGenerationStrategy", NpgsqlValueGenerationStrategy.IdentityByDefaultColumn),
                    Timestamp = table.Column<DateTime>(type: "timestamp with time zone", nullable: false),
                    TotalBytesSent = table.Column<long>(type: "bigint", nullable: false),
                    MemorySavedBytes = table.Column<long>(type: "bigint", nullable: false),
                    ActiveZeroCopyBuffers = table.Column<int>(type: "integer", nullable: false),
                    PeakZeroCopyBuffers = table.Column<int>(type: "integer", nullable: false),
                    AverageBroadcastTimeMs = table.Column<double>(type: "double precision", nullable: false),
                    PeakBroadcastTimeMs = table.Column<double>(type: "double precision", nullable: false),
                    TotalBroadcasts = table.Column<int>(type: "integer", nullable: false),
                    ActiveClients = table.Column<int>(type: "integer", nullable: false),
                    ActiveSources = table.Column<int>(type: "integer", nullable: false),
                    TotalMountPoints = table.Column<int>(type: "integer", nullable: false),
                    TotalMemoryUsageBytes = table.Column<long>(type: "bigint", nullable: false),
                    GcCollectionCount = table.Column<long>(type: "bigint", nullable: false)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_PerformanceMetrics", x => x.Id);
                });
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "PerformanceMetrics");
        }
    }
}
