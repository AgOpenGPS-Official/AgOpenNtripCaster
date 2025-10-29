using System;
using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Migrations
{
    /// <inheritdoc />
    public partial class AddRtcmDataFields : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<int>(
                name: "BytesPerSecond",
                table: "MountPoints",
                type: "integer",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "DetectedFormat",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "DetectedNavSystems",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<DateTime>(
                name: "LastRtcmMessageTime",
                table: "MountPoints",
                type: "timestamp with time zone",
                nullable: true);

            migrationBuilder.AddColumn<int>(
                name: "MessageCount",
                table: "MountPoints",
                type: "integer",
                nullable: false,
                defaultValue: 0);

            migrationBuilder.AddColumn<int>(
                name: "ReferenceStationId",
                table: "MountPoints",
                type: "integer",
                nullable: true);

            migrationBuilder.AddColumn<decimal>(
                name: "RtcmLatitude",
                table: "MountPoints",
                type: "numeric",
                nullable: true);

            migrationBuilder.AddColumn<decimal>(
                name: "RtcmLongitude",
                table: "MountPoints",
                type: "numeric",
                nullable: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "BytesPerSecond",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "DetectedFormat",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "DetectedNavSystems",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "LastRtcmMessageTime",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "MessageCount",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "ReferenceStationId",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "RtcmLatitude",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "RtcmLongitude",
                table: "MountPoints");
        }
    }
}
