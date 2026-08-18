import { computed, inject, Service, signal, WritableSignal } from '@angular/core';
import { Feed } from '@models/feed';
import { environment } from '@environments/environment';
import { JobListing } from '@models/job-listing';
import { HttpService } from '@services/http/http';


const ALL = 'all' as const;

@Service()
export class FeedService {
    private readonly m_feeds: WritableSignal<Feed[]> = signal<Feed[]>([]);
    private readonly m_http: HttpService = inject(HttpService);

    private readonly m_selectedCategory =  signal<string>(ALL);
    private readonly m_selectedListingId = signal<number | null>(null);
    private readonly m_selectedLocation =  signal<string>(ALL);
    private readonly m_searchTerm =        signal('');

    private readonly m_allListings =      computed<JobListing[]>(this.getAllListings.bind(this));
    private readonly m_categories =       computed(() => Array.from(new Set(this.m_allListings().map((l) => l.category))).sort());
    private readonly m_filteredListings = computed<JobListing[]>(this.filterListings.bind(this));
    private readonly m_hasActiveFilters = computed(this.hasActiveFilters_.bind(this));
    private readonly m_locations =        computed(() => Array.from(new Set(this.m_allListings().map((l) => l.location))).sort());
    private readonly m_selectedListing =  computed<JobListing | null>(this.selectedListing_.bind(this));

    public get feeds() {
        return this.m_feeds;
    }

    public get searchTerm() {
        return this.m_searchTerm;
    }

    public get selectedCategory() {
        return this.m_selectedCategory;
    }

    public get selectedLocation() {
        return this.m_selectedLocation;
    }

    public get selectedListingId() {
        return this.m_selectedListingId;
    }

    public get allListings() {
        return this.m_allListings;
    }

    public get categories() {
        return this.m_categories;
    }

    public get locations() {
        return this.m_locations;
    }

    public get filteredListings() {
        return this.m_filteredListings;
    }

    public get hasActiveFilters() {
        return this.m_hasActiveFilters;
    }

    public get selectedListing() {
        return this.m_selectedListing;
    }

    public clearFilters(): void {
        this.m_searchTerm.set('');
        this.m_selectedCategory.set(ALL);
        this.m_selectedLocation.set(ALL);
    }

    public fetchFeed() : void {
        this.m_http.get<Feed[]>(this.buildPath("/feed")).subscribe(this.updateFeeds.bind(this))
    }

    private buildPath(path: string) {
        return `${environment.apiUrl}/rss${path}`;
    }

    private filterListings() {
        const term = this.m_searchTerm().trim().toLowerCase();
        const category = this.m_selectedCategory();
        const location = this.m_selectedLocation();

        return this.m_allListings().filter((listing) => {
            const matchesTerm = !term ||
                                listing.title.toLowerCase().includes(term) ||
                                listing.company.toLowerCase().includes(term);

            const matchesCategory = category === ALL ||
                                    listing.category === category;

            const matchesLocation = location === ALL ||
                                    listing.location === location;

            return matchesTerm &&
                   matchesCategory &&
                   matchesLocation;
        });
    }

    private getAllListings() {
        const byId = new Map<number, JobListing>();
        for (const feed of this.feeds())
            for (const listing of feed.listings)
                if (!byId.has(listing.id))
                    byId.set(listing.id, listing);

        return Array.from(byId.values()).sort((a, b) => b.created_at - a.created_at);
    }

    private hasActiveFilters_() {
        return this.m_searchTerm().trim().length > 0 ||
               this.m_selectedCategory() !== ALL ||
               this.m_selectedLocation() !== ALL;
    }

    private selectedListing_() {
        const id = this.m_selectedListingId();

        if (id === null)
            return null;

        return this.m_allListings().find((listing) => listing.id === id) ?? null;
    }

    private updateFeeds(response: Feed[]) {
        this.m_feeds.update(() => response);
    }
}